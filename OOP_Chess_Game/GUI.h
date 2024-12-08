#pragma once
#include <vector>
#include <String>
#include <SDL.h>
#include "Image.h"
#include "Board.h"
#include "Pieces.h"

#define PIECESIZE 60
#define DISPLACE 40
#define WINDOWSIZEX 640
#define WINDOWSIZEY 560
//
#define SUBDISPLACE 175
#define SUBSIZEX WINDOWSIZEX-SUBDISPLACE*2
#define SUBSIZEY WINDOWSIZEY-SUBDISPLACE*2

enum class GUIType { MENU, MODE_OPTION, VOLUME_OPTION, REPLAY_RECENT_GAME, GAME_PLAY, RESULT_NOTICE, PROMOTION_NOTICE, SETTINGS };
enum class MatchState { BLACK_WIN, WHITE_WIN, DRAW, IN_PLAY};

class GUI {
protected:
	Image* background;
	bool active;
public:
	GUI();
	virtual ~GUI();
	virtual GUIType getGUIType() const;
	void renderBackground();
	virtual void render() = 0;
	virtual void destroy() = 0;
	SDL_Rect getGUIRect();
	bool isActive();
	void activate();
	void deactivate();
};
//
class MenuGUI : public GUI {
protected:
	Image* btnContinueSavedGame;
	Image* btnVsComputerEasy;
	Image* btnVsComputerHard;
	Image* btnVsPlayer;
	Image* btnReplayRecentGame;
	Image* btnExit;
	//
	Image* btnMute;
	Image* btnUnMute;
public:
	MenuGUI();
	~MenuGUI();

	void render();
	void renderMute(bool mute);
	GUIType getGUIType() const;

	SDL_Rect getRectOfBtnContinueSavedGame();
	SDL_Rect getRectOfBtnVsPlayer();
	SDL_Rect getRectOfBtnVsComputerEasy();
	SDL_Rect getRectOfBtnVsComputerHard();
	SDL_Rect getRectOfBtnVolume();
	SDL_Rect getRectOfBtnReplayRecentGame();
	SDL_Rect getRectOfBtnExit();

	void destroy();
};


class GamePlayGUI : public GUI {
protected:
	std::vector<Image*> piece;
	//
	Piece* chosenPiece;
	//
	Image* chosen;
	Image* possibleMove;
	Image* possibleCapture;
	//
	Image* board;
	Image* btnSetting;
	Image* btnUndo;
	Image* btnRedo;
	//
	Image* whiteMove;
	Image* blackMove;
	//
public:
	GamePlayGUI();
	//
	~GamePlayGUI();
	//
	GUIType getGUIType() const;
	//
	void set();
	void render();
	void renderTurn(int turn);
	//
	SDL_Rect getRectOfBoard();
	SDL_Rect getRectOfBtnSetting();
	SDL_Rect getRectOfBtnUndo();
	SDL_Rect getRectOfBtnRedo();
	SDL_Rect getRectOfBtnQuit() {
		return { 0,0,0,0 };
	}
	//
	void destroy();
	//
};

class MatchResultGUI : public GUI {
private:
	Image* result;
	Image* btnPlayAgain;
	Image* btnBackToMenu;
	Image* btnSave;
public:
	MatchResultGUI();
	MatchResultGUI(MatchState ms);
	~MatchResultGUI();

	void render();
	GUIType getGUIType() const;
	void destroy();

	SDL_Rect getRectOfBtnPlayAgain();
	SDL_Rect getRectOfBtnBackToMenu();
	SDL_Rect getRectOfBtnSave();
};
////////////////
class PromotionGUI : public GUI {
private:
	Image* btnQueen;
	Image* btnBishop;
	Image* btnKnight;
	Image* btnRook;
	Image* up;
public:
	PromotionGUI();
	~PromotionGUI();

	void render();
	GUIType getGUIType() const;
	void destroy();

	SDL_Rect getRectOfBtnQueen();
	SDL_Rect getRectOfBtnBishop();
	SDL_Rect getRectOfBtnKnight();
	SDL_Rect getRectOfBtnRook();
};

class SettingGUI : public GUI {
private:
	Image* symbol;
	Image* btnResume;
	//
	Image* btnMute;
	Image* btnUnMute;
	//
	Image* btnBackToMenu;
	Image* btnSave;
public:
	SettingGUI();
	~SettingGUI();

	void render();
	void renderMute(bool mute);
	GUIType getGUIType() const;
	void destroy();

	SDL_Rect getRectOfBtnResume();
	SDL_Rect getRectOfBtnVolume();
	SDL_Rect getRectOfBtnBackToMenu();
	SDL_Rect getRectOfBtnSave();
};

