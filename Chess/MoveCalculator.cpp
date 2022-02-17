#include "MoveCalculator.h"


Points MoveCalculator::get_all_moves(const _Board& board, const Piece& p) const noexcept
{
	Points v;

	switch (p.type) {
	case TYPE::bishop:
		v = bishop(board, p);
		break;
	case TYPE::king:
		v = king(board, p);
		break;
	case TYPE::pawn:
		v = pawn(board, p);
		break;
	case TYPE::rook:
		v = rook(board, p);
		break;
	case TYPE::knight:
		v = knight(board, p);
		break;
	case TYPE::queen:
		v = queen(board, p);
		break;
	default:
		v = {};
		break;
	}
	return v;
}

void MoveCalculator::simulate_move(_Board& board, Piece p, const SDL_Point& to) const noexcept
{
	auto& v = board[p.x][p.y];
	v.set_position(to.x, to.y);
	v.has_moved = true;
	board[to.x][to.y] = v;
	board[p.x][p.y] = Piece(p.x, p.y, -1);
}

MoveCalculator::MoveCalculator(const _Board& board, const SDL_Point& two_pawn_advancedment): BOARD(board), previous_state(), previous_moves(), previous_point(), two_pawn_advancedment(two_pawn_advancedment){}

Points MoveCalculator::bishop(const _Board& board, const Piece& p) const noexcept
{
	Points v{};
	walk_append(board, v, p, 1, 1);
	walk_append(board, v, p, -1, -1);
	walk_append(board, v, p, 1, -1);
	walk_append(board, v, p, -1, 1);
	return v;
}

Points MoveCalculator::pawn(const _Board& board, const Piece& p) const noexcept
{
	Points v{};
	int dir = p.player == PLAYER::black ? 1 : -1;
	if (append_if_empty(board, v, p.x, p.y + dir)) {
		if (!p.has_moved) { append_if_empty(board, v, p.x, p.y + 2 * dir); }
	}
	append_if_enemy(board, v, p.x - 1, p.y + dir, p);
	append_if_enemy(board, v, p.x + 1, p.y + dir, p);
	// en passent
	auto& [px, py] = two_pawn_advancedment;
	if (px == -1 || board[px][py].player == p.player) {
		return v;
	}
	if (p.y == py && std::abs(px - p.x) == 1) {
		v.push_back({ px, py + dir });
	}
	return v;
}

Points MoveCalculator::queen(const _Board& board, const Piece& p) const noexcept
{
	auto r = rook(board,p);
	auto b = bishop(board, p);
	r.insert(r.begin(), b.begin(), b.end());
	return r;
}

Points MoveCalculator::king(const _Board& board, const Piece& p) const noexcept
{

	Points possible{};
	append_if_not_player(board, possible, p.x + 1, p.y + 1, p);
	append_if_not_player(board, possible, p.x, p.y + 1, p);
	append_if_not_player(board, possible, p.x + 1, p.y, p);
	append_if_not_player(board, possible, p.x - 1, p.y - 1, p);
	append_if_not_player(board, possible, p.x, p.y - 1, p);
	append_if_not_player(board, possible, p.x - 1, p.y, p);
	append_if_not_player(board, possible, p.x - 1, p.y+1, p);
	append_if_not_player(board, possible, p.x+1, p.y-1, p);

	if (p.has_moved) {
		return possible;
	}

	// TODO move rook too
	for (int dir = 1; dir >= -1; dir -= 2) {
		bool check = is_empty(board, p.x + dir, p.y) && is_empty(board, p.x + 2 * dir, p.y) && !board[p.x + (dir == 1 ? 3 : -4)][p.y].has_moved;
		check = dir == 1 ? check : check && is_empty(board, p.x + 3 * dir, p.y);
		if (!check) {
			continue;
		}
		_Board v = board;
		simulate_move(v, p, { p.x + dir, p.y });
		auto first = is_check(v, p.player);
		simulate_move(v, { p.x + dir, p.y, p.type, p.player }, { p.x + 2 * dir, p.y });
		if (!first && !is_check(v, p.player)) {
			possible.push_back({ p.x + 2 * dir, p.y });
		}
		
	}
	// TODO remove castling if checked
	return possible;
}

Points MoveCalculator::rook(const _Board& board, const Piece& p) const noexcept
{
	Points v{};
	// horizontal
	walk_append(board, v, p, 1, 0);
	walk_append(board, v, p, -1, 0);
	walk_append(board, v, p, 0, -1);
	walk_append(board, v, p, 0, 1);


	return v;
}

Points MoveCalculator::knight(const _Board& board, const Piece& p) const noexcept
{
	Points v{};
	append_if_not_player(board, v, p.x + 2, p.y + 1, p);
	append_if_not_player(board, v, p.x - 2, p.y - 1, p);
	append_if_not_player(board, v, p.x - 2, p.y + 1, p);
	append_if_not_player(board, v, p.x + 2, p.y - 1, p);

	append_if_not_player(board, v, p.x + 1, p.y + 2, p);
	append_if_not_player(board, v, p.x - 1, p.y - 2, p);
	append_if_not_player(board, v, p.x - 1, p.y + 2, p);
	append_if_not_player(board, v, p.x + 1, p.y - 2, p);
	return v;
}

void MoveCalculator::walk_append(const _Board& board, Points& v, const Piece& p, const int& xadd, const int& yadd) const noexcept
{
	int x = p.x + xadd; int y = p.y+yadd;
	while (!is_occupied(board, x, y) && is_inbounds(x, y)) {
		v.push_back({ x, y });
		x += xadd;
		y += yadd;
	}
	if (is_enemy(board, x, y, p)) {
		v.push_back({ x, y });
	}
}

bool MoveCalculator::isnt_player(const _Board& board, const int& x, const int& y, const Piece& p) const noexcept
{
	return is_inbounds(x, y) && board[x][y].player != p.player;
}

bool MoveCalculator::append_if_not_player(const _Board& board, Points& v, const int& x, const int& y, const Piece& p) const noexcept
{
	if (isnt_player(board, x, y, p)) {
		v.push_back({ x, y });
		return true;
	}
	return false;
}

bool MoveCalculator::is_check(const _Board& b, const PLAYER& player) const noexcept
{
	for (auto& arr : b) {
		for (auto& p : arr) {
			if (p.player == player || p.player == PLAYER::none) {	
				continue;
			}
			auto v = get_all_moves(b, p);
			for (auto& point : v) {
				auto& k = b[point.x][point.y];
				if (k.player == player && k.type == TYPE::king) {
					return true;
				}
			}
		}
	}
	return false;
}


void MoveCalculator::remove_checks(Points& v, const Piece& p) noexcept
{
	Points new_moves{};
	std::copy_if(v.begin(), v.end(), std::back_inserter(new_moves), [&](SDL_Point& point) {
		auto b = BOARD;
		simulate_move(b, p, point);
		return !is_check(b, p.player);
	});
	v = new_moves;
}

bool MoveCalculator::is_empty(const _Board& board, const int& x, const int& y) const noexcept
{
	return !is_occupied(board, x, y) && is_inbounds(x, y);
}

Points MoveCalculator::get_allowed(const int& x, const int& y) noexcept
{	
	// used for multiple calls 
	if (previous_state == BOARD && previous_point.y == y && previous_point.x == x) {
		return previous_moves;
	}
	previous_state = BOARD;
	previous_point = SDL_Point{ x, y };
	auto& p = BOARD[x][y];
	auto v = get_all_moves(BOARD, p);
	remove_checks(v, p);
	previous_moves = v;
	return v;
}


bool MoveCalculator::append_if_enemy(const _Board& board, Points& v, const int& x, const int& y, const Piece& p) const noexcept
{
	if (is_enemy(board, x, y, p)) {
		v.push_back({ x, y });
		return true;
	}
	return false;
}

bool MoveCalculator::is_occupied(const _Board& board, const int& x, const int& y) const noexcept {
	return is_inbounds(x, y) && !board[x][y].is_empty();
}

bool MoveCalculator::is_enemy(const _Board& board, const int& x, const int& y, const Piece& p) const noexcept
{
	return is_inbounds(x, y) && board[x][y].player != p.player && is_occupied(board, x, y);
}

bool MoveCalculator::is_inbounds(const int& x, const int& y) const noexcept
{
	return x >= 0 && x < 8 && y >= 0 && y < 8;
}


bool MoveCalculator::append_if_empty(const _Board& board, Points& v, const int& x, const int& y) const noexcept {
	if (is_empty(board, x, y)) {
		v.push_back({ x, y });
		return true;
	}
	return false;
}