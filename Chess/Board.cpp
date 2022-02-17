#include "Board.h"

int Board::get_square_width() const noexcept {
	return surfaces[{TYPE::none, PLAYER::black}]->w;
}

Board::Board(std::map<std::pair<TYPE, PLAYER>, SDL_Surface*>& surfaces) : surfaces(surfaces), 
board(Piece::parse_string("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR")),
surface(NULL), offset({0, 0}), highlight(NULL), calc(board, two_pawn_advancedment), danger(NULL), current_player(PLAYER::white),
two_pawn_advancedment({ -1, -1 }) {
	auto& surf = surfaces[{TYPE::none, PLAYER::black}];
	surface = SDL_CreateRGBSurface(surf->flags, board.size() * surf->w, board[0].size() * surf->h, 32, 0, 0, 0, 0);
	for (auto& arr : board) {
		for (auto& p : arr) {
			p.set_width(get_square_width());
		}
	}
	selected_piece = SDL_Point{ -1, -1 };
	
	highlight = SDL_CreateRGBSurface(surf->flags, surf->w, surf->h, 32, 0, 0, 0, 0);
	danger = SDL_CreateRGBSurface(surf->flags, surf->w, surf->h, 32, 0, 0, 0, 0);

	// format is b g r a
	SDL_FillRect(highlight, NULL, SDL_MapRGB(surface->format, 255, 255, 0));
	SDL_FillRect(danger, NULL, SDL_MapRGB(surface->format, 255, 0, 0));

	constexpr auto alpha = 120;

	SDL_SetSurfaceAlphaMod(highlight, alpha);
	SDL_SetSurfaceBlendMode(highlight, SDL_BLENDMODE_BLEND);	
	SDL_SetSurfaceAlphaMod(danger, alpha);
	SDL_SetSurfaceBlendMode(danger, SDL_BLENDMODE_BLEND);

}

Board::~Board() {
	SDL_FreeSurface(surface);
	SDL_FreeSurface(highlight);
	SDL_FreeSurface(danger);
}

void Board::set_board(const std::string_view& s) {
	this->board = Piece::parse_string(s);
	
	for (auto& arr : board) {
		for (auto& p : arr) {
			p.set_width(get_square_width());
		}
	}
}

SDL_Surface* Board::get_surface() {

	// squares
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			auto pair = (x + y) % 2 == 0 ? std::pair(TYPE::none, PLAYER::white) : std::pair(TYPE::none, PLAYER::black);
			auto& surf = surfaces[pair];
			auto rect = point_bounds(x, y);

			SDL_UpperBlit(surf, NULL, surface, &rect);
		}
	}

	// highlight
	if (selected_piece.x != -1) {
		auto r = point_bounds(selected_piece.x, selected_piece.y);
		SDL_UpperBlit(highlight, NULL, surface, &r);
		for (auto& [x, y] : calc.get_allowed(selected_piece.x, selected_piece.y)) {
			auto r = point_bounds(x, y);
			SDL_UpperBlit(highlight, NULL, surface, &r);
		}
	}



	// pieces
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			auto& p = board[x][y];
			auto& s = surfaces[{p.type, p.player}];
			auto r = p.get_rect();
			if (p.type == TYPE::king && calc.is_check(board, p.player)) {
				SDL_UpperBlit(danger, NULL, surface, &r);
			}
			SDL_UpperBlit(s, NULL, surface, &r);
		}
	}

	return surface;
}

SDL_Rect Board::get_rect()
{
	return { offset.x, offset.y, surface->w, surface->h };
}

void Board::click(const SDL_Point& coord) {
	auto [x, y] = coord;
	x -= offset.x;
	y -= offset.y;
	// swapped for internal array
	x /= get_square_width();
	y /= get_square_width();
	if (x >= 8 || x < 0 || y >= 8 || y < 0) {
		return;
	}
	on_click(x, y);
}

SDL_Rect Board::point_bounds(const int& x, const int& y) const noexcept
{
	auto& w = board[0][0].pixel_width;
	return {x*w, y*w, w, w};
}

bool Board::is_checkmate(const PLAYER& player) noexcept {
	for (auto& vec : board) {
		for (auto& p : vec) {
			if (p.player != player) {
				continue;
			}
			for (auto& move : calc.get_allowed(p.x, p.y)) {
				if (is_valid_move(p.x, p.y, move)) {
					return false;
				}
			}
		}
	}
	return true;
}

void Board::move(const SDL_Point& from, const SDL_Point& to) noexcept {
	auto& v = board[from.x][from.y];
	v.set_position(to.x, to.y);
	v.has_moved = true;
	board[to.x][to.y] = v;
	board[from.x][from.y] = Piece(from.x, from.x, get_square_width());

	if (board[to.x][to.y].type == TYPE::pawn && std::abs(from.y - to.y) > 1) {
		two_pawn_advancedment = { to.x, to.y };
	}
	else {
		two_pawn_advancedment = { -1, -1 };
	}

}

void Board::on_click(const int& x, const int& y) {
	auto& [px, py] = selected_piece;
	if (board[x][y].player != current_player && px == -1 && py == -1) {
		return;
	}

	// select a piece
	if (px == -1 && py == -1) {
		px = x; py = y;
		return;
	}
	// deselect
	if (px == x && py == y) {
		px = -1; py = -1; return;
	}
	// different piece
	if (board[px][py].player == board[x][y].player) {
		px = x; py = y; return;
	}
	// move a piece
	if (!is_valid_move(px, py, { x, y })) {
		return;
	}
	// en passant
	if (board[px][py].type == TYPE::pawn && board[x][y].is_empty() && x != px) {
		auto& [tx, ty] = two_pawn_advancedment;
		board[tx][ty] = Piece(tx, ty, board[tx][ty].pixel_width);
	}
	move({ px, py }, { x, y });

	// upgrade board
	if (board[x][y].type == TYPE::pawn && (y == 0 || y == board.size() -1)) {
		// cant move a pawn backwards so you dont have to check
		auto p = Piece(x, y, TYPE::queen, board[x][y].player);
		p.set_width(board[x][y].pixel_width);
		board[x][y] = p;
	}

	// moves rook
	auto diff = px - x;
	if (board[x][y].type == TYPE::king && std::abs(diff) > 1) {
		auto index = diff > 1 ? 0 : 7;
		auto place = diff > 1 ? 3 : 5;
		move({ index, y }, { place, y });
	}
	px = -1; py = -1;
	
	if (is_checkmate(PLAYER::white)) {
		std::cout << "Checkmate YOU LOSE\n";
	}
	if (is_checkmate(PLAYER::black)) {
		std::cout << "Checkmate YOU WIN\n";
	}

	current_player = current_player == PLAYER::black ? PLAYER::white : PLAYER::black;


}

bool Board::is_valid_move(const int& x, const int& y, const SDL_Point& point) noexcept {
	for (auto& el : calc.get_allowed(x, y)) {
		if (el.x == point.x && el.y == point.y) {
			if (board[x][y].type == TYPE::none) {
				return false;
			}
			// cant castle if checked
			if (board[x][y].type == TYPE::king && calc.is_check(board, board[x][y].player) && std::abs(point.x - x) > 1) {
				return false;
			}
			return true;
		}
	}
	return false;
}