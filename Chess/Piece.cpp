#include "Piece.h"

Piece::Piece(const int& x, const int& y, const int& pixel_width) {
	this->x = x; this->y = y;
	this->has_moved = false;
	this->type = TYPE::none;
	this->player = PLAYER::none;
	this->pixel_width = pixel_width;
	this->pixel_coord = SDL_Point{ x * pixel_width, y * pixel_width };
}

Piece::Piece() {
	x = -1; y = -1; pixel_width = -1;
	type = TYPE::none; player = PLAYER::none;
	pixel_coord = SDL_Point{ -1, -1 };
	has_moved = false;
}

/*
* Returns type and player of a character GIVEN THAT IT IS AN
* ALPHA CHARACTER
*/
std::pair<TYPE, PLAYER> parse_alpha(const char c) {
	TYPE type;
	switch (tolower(c)) {
	case 'r':
		type = TYPE::rook;
		break;
	case 'k':
		type = TYPE::king;
		break;
	case 'b':
		type = TYPE::bishop;
		break;
	case 'q':
		type = TYPE::queen;
		break;
	case 'p':
		type = TYPE::pawn;
		break;
	case 'n':
		type = TYPE::knight;
		break;
	default:
		type = TYPE::none;
		break;
	}
	return { type, islower(c) ? PLAYER::black : PLAYER::white };
}

std::array<std::array<Piece, 8>, 8> Piece::parse_string(const std::string_view& s) {
	std::array<std::array<Piece, 8>, 8> arr = {};
	int x = 0; int y = 0;
	for (auto& c : s) {
		// next y
		if (c == '/') {
			y++;
			x = 0;
			continue;
		}
		if (isalpha(c)) {
			auto p = parse_alpha(c);
			arr[x][y] = Piece(x, y, p.first, p.second);
			x++;
			continue;
		}
		// must be numeric spaces then
		int skip = c - '0';
		x += skip;
	}
	return arr;
	
}

bool Piece::operator==(const Piece& o) const noexcept {
	return x == o.x && type == o.type && player == o.player && pixel_width == o.pixel_width;
}

void Piece::set_width(const int& w) noexcept {
	this->pixel_width = w;
	this->pixel_coord = SDL_Point{ x * w, y * w};
}

bool Piece::is_empty() const noexcept {
	return type == TYPE::none && player == PLAYER::none;
}

std::string_view type_string(TYPE type) {
	switch (type) {
	case TYPE::rook: return "rook";
	case TYPE::bishop: return "bishop";
	case TYPE::king: return "king";
	case TYPE::knight: return "knight";
	case TYPE::none: return "none";
	case TYPE::pawn: return "pawn";
	case TYPE::queen: return "queen";
	}
}

std::string_view player_string(PLAYER player) {
	switch (player) {
	case PLAYER::black: return "black";
	case PLAYER::white: return "white";
	case PLAYER::none: return "none";
	}
}

void Piece::set_position(const int& x, const int& y) {
	this->x = x; this->y = y;
	set_width(pixel_width);
}