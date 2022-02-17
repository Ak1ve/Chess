#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include<array>
#include <string_view>
#include <stdio.h>
#include <iostream>
#include <ctype.h>

enum class TYPE {
	rook,
	pawn,
	king,
	queen,
	bishop,
	knight,
	none
};

std::string_view type_string(TYPE type);

enum class PLAYER {
	black,
	white,
	none
};

std::string_view player_string(PLAYER type);

class Piece
{
	// location on the surface.
	SDL_Point pixel_coord;
public:
	int pixel_width;
	// type of piece it is
	TYPE type;
	// player
	PLAYER player;
	// location on the board
	int x, y;
	// used for pawns and such
	bool has_moved;
	/*
	* x and y is the POSITION on the board
	* default is an TYPE::none, PLAYER::none
	* 
	*/
	Piece(const int& x, const int& y, const int& pixel_width);

	/*
	* Constructor mainly used for parse_string MUST CALL set_width() TO ALL FOR 
	* DRAWING
	*/
	Piece(const int& x, const int& y, TYPE type, PLAYER player) : x(x), y(y), pixel_coord(), has_moved(false), pixel_width(-1), type(type), player(player) {}

	/*
	* Empty piece
	*/
	Piece();

	/*
	* Returns if the piece does not have anything on it
	*/
	bool is_empty() const noexcept;

	bool operator==(const Piece& o) const noexcept;

	/*
	* Sets the pixel width
	*/
	void set_width(const int& w) noexcept;

	/*
	* Gets rect
	*/
	SDL_Rect get_rect() const noexcept {
		return { pixel_coord.x, pixel_coord.y, pixel_width, pixel_width };
	}

	/*
	* Call when moving to a new position on the BOARD
	*/
	void set_position(const int& x, const int& y);

	/*
	* https://www.chess.com/terms/fen-chess#piece-placement FEN
	* to array board
	* MUST CALL set_width() FOR DRAWING!!
	*/
	static std::array<std::array<Piece, 8>, 8> parse_string(const std::string_view& s);
	

};

