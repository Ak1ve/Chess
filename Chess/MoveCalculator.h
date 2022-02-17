#pragma once
#include <vector>
#include <SDL.h>
#include <array>
#include "Piece.h"
#include <algorithm>
#include <iostream>

using _Board = std::array<std::array<Piece, 8>, 8>;
using Points = std::vector<SDL_Point>;

class MoveCalculator
{
	/*
	* DOES NOT INCLUDE CHECKS
	*/
	Points get_all_moves(const _Board& board, const Piece& p) const noexcept;
	/*
	* simulates a move, used for simulating checks.  mutates the given board.
	* Assumes the move is valid
	*/
	void simulate_move(_Board& board, Piece p, const SDL_Point& to) const noexcept;

public:
	const _Board& BOARD;
	const SDL_Point& two_pawn_advancedment;
	_Board previous_state;
	Points previous_moves;
	SDL_Point previous_point;


	MoveCalculator(const _Board& board, const SDL_Point& two_pawn_advancedment);

	/*
	* For bishops
	*/
	Points bishop(const _Board& board, const Piece& p) const noexcept;

	Points pawn(const _Board& board, const Piece& p) const noexcept;

	Points queen(const _Board& board, const Piece& p) const noexcept;

	Points king(const _Board& board, const Piece& p) const noexcept;

	Points rook(const _Board& board, const Piece& p) const noexcept;

	Points knight(const _Board& board, const Piece& p) const noexcept;

	/*
	* Appends to the given vector if the board at xy is empty.  Returns if the
	* board was empty 
	*/
	bool append_if_empty(const _Board& board, Points& v, const int& x, const int& y) const noexcept;

	/*
	* Appends to the given vector if the board at xy is an enemy piece.  Returns true
	* if the board was an enemy
	*/
	bool append_if_enemy(const _Board& board, Points& v, const int& x, const int& y, const Piece& p) const noexcept;

	/*
	* if the position is occupied by a piece
	*/
	bool is_occupied(const _Board& board, const int& x, const int& y) const noexcept;

	/*
	* Returns true if the space is occupied by an enemy
	*/
	bool is_enemy(const _Board& board, const int& x, const int& y, const Piece& p) const noexcept;

	/*
	* Basically just checks x and y 0 <= x < 8
	*/
	bool is_inbounds(const int& x, const int& y) const noexcept;

	/*
	* Does a while loop that walks p.x + xadd, p.y + yadd
	* appends to the vec if the point is empty, or an enemy is located
	*/
	void walk_append(const _Board& board, Points& v, const Piece& p, const int& xadd, const int& yadd) const noexcept;

	/*
	* If the place is in bounds and isnt by the same player
	*/
	bool isnt_player(const _Board& board, const int& x, const int& y, const Piece& p) const noexcept;

	/*
	* Appends if isnt player.  True if did append
	*/
	bool append_if_not_player(const _Board& board, Points& v, const int& x, const int& y, const Piece& p) const noexcept;
	
	/*
	* checks if the given board is check
	*/
	bool is_check(const _Board& b, const PLAYER& player) const noexcept;

	/*
	* Removes all moves that result in checks
	* re
	*/
	void remove_checks(Points& v, const Piece& p) noexcept;

	bool is_empty(const _Board& board, const int& x, const int& y) const noexcept;

	/*
	* Gets allowed pieces of a certain spot at the board
	*/
	Points get_allowed(const int& x, const int& y) noexcept;


};

