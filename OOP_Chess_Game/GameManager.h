#pragma once
#include <iostream>
#include <string>
#include <SDL.h>

#include "SoundManager.h"
#include "GUI.h" // TODO: check
#include "Board.h"
#include "History.h"
#include "Computer.h"
#include "Coordinate.h"
#include "Window.h"

enum class MatchResult { PLAYING, PLAYER1_WIN, PLAYER2_WIN, DRAW };
enum class GamePlayGUIState {DISPLAY, PLAY};


class GameManager {
private:
	// control model
	Board* board;
	SoundManager* soundManager;
	Computer* computer;
	History* history;

	// control view
	GUI* mainGui;
	GUI* subGui;

	// flags
	bool isRunning;
	Opponent opponent;
	int turn;
	GamePlayGUIState state;
	long long cnt;
public:


	GameManager(const char* title, int xPos, int yPos, int width, int height);
	~GameManager();

	void gameLoop(int fps);

	void handelEvents();

	void render();

	Coordinate getClickedBox(const SDL_Event& e) const;

	void handleClickedPiece(const SDL_Event& e);

	void handleClickedHighlightBox(const SDL_Event& e);

	bool checkFocus(const SDL_Event& e, const SDL_Rect& rect) const;

	bool checkPromotion(Piece* piece);

	void promote(PieceType type);

	void undo();

	void redo();

	void resetGame();

	void saveCurrentGame(std::string path);

	void loadPreviousGame(std::string path);

	MatchState checkWinner();

	void recoverGameFromHistory();
};

