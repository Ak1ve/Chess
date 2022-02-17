#pragma once
#include <array>
#include <SDL.h>
#include "Piece.h"
#include <map>
#include <vector>
#include "MoveCalculator.h"

class Board
{
	SDL_Surface* surface;
	std::map<std::pair<TYPE, PLAYER>, SDL_Surface*>& surfaces;
	// allowed moves
	std::vector<SDL_Point> highlighted;
	
	MoveCalculator calc;

	SDL_Surface* danger;
	SDL_Point selected_piece;
	SDL_Surface* highlight;
	PLAYER current_player;
	// keeps track of en passent
	SDL_Point two_pawn_advancedment;

	int get_square_width() const noexcept;

	void on_click(const int& x, const int& y);

public:
	// offset of the board in relation to the window
	SDL_Point offset;
	// grid is [x][y]
	std::array<std::array<Piece, 8>, 8> board;


	Board(std::map<std::pair<TYPE, PLAYER>, SDL_Surface*>& surfaces);
	~Board();
	/*
	* Returns blitted surface
	*/
	SDL_Surface* get_surface();

	SDL_Rect get_rect();

	void set_board(const std::string_view& s);

	SDL_Rect point_bounds(const int& x, const int& y) const noexcept;
	
	/*
	* Returns true if the move is valid
	*/
	bool is_valid_move(const int& x, const int& y, const SDL_Point& point) noexcept;

	/*
	* Does not check if its a valid move
	*/
	void move(const SDL_Point& from, const SDL_Point& to) noexcept;

	bool is_checkmate(const PLAYER& player) noexcept;

	/*
	* Coord is pixels in relation to the window
	*/
	void click(const SDL_Point& coord);
};

