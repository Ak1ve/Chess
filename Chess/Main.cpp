#include <iostream>
#include <SDL.h>
#include "SDL_Image.h"
#include <map>
#include "Board.h"


int main(int argc, char** args) {
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		std::cout << "Error initializing SDL: " << SDL_GetError() << '\n';
		system("pause");
		return 1;
	}
	
	SDL_Window* window = SDL_CreateWindow("Title", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 80*8, 80*8, 0);
	SDL_Surface* winSurf = SDL_GetWindowSurface(window);
	SDL_Event ev;
	SDL_SetSurfaceAlphaMod(winSurf, 255);
	std::map<std::pair<TYPE, PLAYER>, SDL_Surface*> surfaces;
	surfaces.insert(std::pair(std::pair(TYPE::none, PLAYER::black), IMG_Load("images/black_tile.png")));
	surfaces.insert(std::pair(std::pair(TYPE::none, PLAYER::white), IMG_Load("images/white_tile.png")));
	surfaces.insert(std::pair(std::pair(TYPE::rook, PLAYER::black), IMG_Load("images/br.png")));
	surfaces.insert(std::pair(std::pair(TYPE::pawn, PLAYER::black), IMG_Load("images/bp.png")));
	surfaces.insert(std::pair(std::pair(TYPE::king, PLAYER::black), IMG_Load("images/bk.png")));
	surfaces.insert(std::pair(std::pair(TYPE::queen, PLAYER::black), IMG_Load("images/bq.png")));
	surfaces.insert(std::pair(std::pair(TYPE::bishop, PLAYER::black), IMG_Load("images/bb.png")));
	surfaces.insert(std::pair(std::pair(TYPE::knight, PLAYER::black), IMG_Load("images/bn.png")));
	surfaces.insert(std::pair(std::pair(TYPE::rook, PLAYER::white), IMG_Load("images/wr.png")));
	surfaces.insert(std::pair(std::pair(TYPE::pawn, PLAYER::white), IMG_Load("images/wp.png")));
	surfaces.insert(std::pair(std::pair(TYPE::king, PLAYER::white), IMG_Load("images/wk.png")));
	surfaces.insert(std::pair(std::pair(TYPE::queen, PLAYER::white), IMG_Load("images/wq.png")));
	surfaces.insert(std::pair(std::pair(TYPE::bishop, PLAYER::white), IMG_Load("images/wb.png")));
	surfaces.insert(std::pair(std::pair(TYPE::knight, PLAYER::white), IMG_Load("images/wn.png")));
		
	// TODO pawn stuff, castling, pawn upgrades
	Board board{ surfaces };
	// default is "rnbqkbnr/ppppppp/8/8/8/8/8/RNBQKBNR"
	board.set_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");

	auto running = true;
	while (running) {
		while (SDL_PollEvent(&ev)) {
			switch (ev.type) {
			case SDL_QUIT:
				running = false;
				break;
			case SDL_MOUSEBUTTONDOWN:
				board.click({ ev.button.x, ev.button.y });
				break;
			}
		}
		auto r = board.get_rect();
		SDL_UpperBlit(board.get_surface(), NULL, winSurf, &r);
		SDL_UpdateWindowSurface(window);
		SDL_Delay(100);

	}

	for (auto& v : surfaces) {
		SDL_FreeSurface(v.second);
	}

	SDL_DestroyWindow(window);
	return 0;


}