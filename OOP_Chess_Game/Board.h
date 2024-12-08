#pragma once
#include "Pieces.h"
#include <vector>
#include <SDL.h>

class Board {
private:
	static Board* _instance;
	SDL_Rect rect;
	Board();
public:
	static std::vector<Piece*> piecesList;
	static std::vector<std::vector<Piece*>> piecesOnBoard;

	~Board();
	static const int XBOXES = 8;
	static const int YBOXES = 8;

	static Board* getInstance();
	static void removeInstance();
	static void updateBoard();
	static Piece* getPieceAt(const Coordinate& c);
	static void resetPiecesList();
};
