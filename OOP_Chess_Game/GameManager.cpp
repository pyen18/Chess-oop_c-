#include "GameManager.h"

GameManager::GameManager(const char* title, int xPos, int yPos, int width, int height) {

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) throw SDL_GetError();

	std::cout << "Subsystems Initialized! ... " << std::endl;

	Window::window = SDL_CreateWindow(title, xPos, yPos, width, height, 0);
	if (!Window::window) throw SDL_GetError();
	std::cout << "Window created!" << std::endl;

	Window::renderer = SDL_CreateRenderer(Window::window, -1, 0);
	if (!Window::renderer) throw SDL_GetError();
	std::cout << "Renderer created!" << std::endl;

	Window::SCREEN_HEIGHT = height;
	Window::SCREEN_WIDTH = width;

	soundManager = new SoundManager();
	board = Board::getInstance();
	computer = new Computer();
	history = new History();
	mainGui = new MenuGUI();
	subGui = nullptr;

	isRunning = true;
	opponent = Opponent::HUMAN; // default
	turn = 0; // start game, player1: 0 -> white; palyer2: 1->black
	state = GamePlayGUIState::PLAY; // default;
	cnt = 0;
}

GameManager::~GameManager() {
	delete soundManager, computer, history, mainGui, subGui;
	Board::removeInstance();
	board = nullptr; soundManager = nullptr; computer = nullptr;  history = nullptr; mainGui = nullptr; subGui = nullptr;
	SDL_DestroyRenderer(Window::renderer);
	SDL_DestroyWindow(Window::window);
	SDL_Quit();
}

void GameManager::gameLoop(int fps) {

	int frameDelay = 1000 / fps;
	Uint32 frameStart = 0;
	int frameTime = 0;

	soundManager->playMainMenuMusic();

	while (isRunning) {
		frameStart = SDL_GetTicks();
		handelEvents(); // not here
		render();
		frameTime = SDL_GetTicks() - frameStart;
		//std::cout << "FrameTime: " << frameTime << std::endl;
		if (frameDelay > frameTime) {
			SDL_Delay(frameDelay - frameTime);
		}
	}
}

void GameManager::render() {
	SDL_RenderClear(Window::renderer);
	mainGui->render();
	if (mainGui->getGUIType() == GUIType::GAME_PLAY) {
		GamePlayGUI* tempGui = (GamePlayGUI*)mainGui;
		tempGui->renderTurn(turn);
	}
	if (mainGui->getGUIType() == GUIType::MENU) {
		MenuGUI* tempGui = (MenuGUI*)mainGui;
		tempGui->renderMute(soundManager->getIsMuted());
	}
	if (subGui) {
		subGui->render();
		if(subGui->getGUIType() == GUIType::SETTINGS){
			SettingGUI* tempGui = (SettingGUI*)subGui;
			tempGui->renderMute(soundManager->getIsMuted());
		}
	}
	SDL_RenderPresent(Window::renderer);
}

// TODO: try-catch
void GameManager::handelEvents() {
	SDL_Event e;

	//check winner
	if (mainGui->getGUIType() == GUIType::GAME_PLAY) {
		MatchState matchState = checkWinner();
		if (matchState != MatchState::IN_PLAY && !subGui) {
			subGui = new MatchResultGUI(matchState);
			if (matchState == MatchState::DRAW) soundManager->playDrawSound();
			else soundManager->playWinSound();
		}

		// default computer: first => white
		if (!subGui && state == GamePlayGUIState::PLAY && (opponent == Opponent::HARD_COMPUTER || opponent == Opponent::EASY_COMPUTER) && turn % 2 == 0 && matchState == MatchState::IN_PLAY) {
			std::pair<int, Coordinate> res = (opponent == Opponent::HARD_COMPUTER) ? computer->playWithHardMode() : computer->playWithEasyMode();
			Piece* piece = Board::piecesList[res.first];
			Piece* capturedPiece = nullptr;
			history->setInitialState(piece);
			capturedPiece = piece->move(res.second, Board::piecesOnBoard);
			history->setFinalState(piece);
			history->setCapturedPiece(capturedPiece);
			history->updateData(turn);
			Board::updateBoard();
			SDL_Delay(100);
			soundManager->playCapturingSound();
			turn++;
			// auto promote to queen
			if (checkPromotion(piece)) {
				promote(PieceType::Queen);
			}
		}

		if (!subGui && state == GamePlayGUIState::DISPLAY) {
			if (cnt >= 60) {
				if (turn < history->getLengthData()) soundManager->playCapturingSound();
				redo();
				cnt = 0;
			}
			cnt++;
		}
	}

	while (SDL_PollEvent(&e)) {
		switch (e.type) {
		case SDL_QUIT:
			isRunning = false; break;
		case SDL_MOUSEBUTTONDOWN:		
			if (subGui) {
				// PROMOTION_NOTICE
				if (subGui->getGUIType() == GUIType::PROMOTION_NOTICE) {
					PromotionGUI* temp = (PromotionGUI*)subGui;
					bool flag = false;

					if (checkFocus(e, temp->getRectOfBtnQueen())) {
						std::cout << "Promote Queen!!!" << std::endl;
						flag = true;
						promote(PieceType::Queen);
					}
					if (checkFocus(e, temp->getRectOfBtnRook())) {
						std::cout << "Promote Rook!!!" << std::endl;
						flag = true;
						promote(PieceType::Rook);
					}
					if (checkFocus(e, temp->getRectOfBtnKnight())) {
						std::cout << "Promote Knight!!!" << std::endl;
						flag = true;
						promote(PieceType::Knight);
					}
					if (checkFocus(e, temp->getRectOfBtnBishop())) {
						std::cout << "Promote Bishop!!!" << std::endl;
						flag = true;
						promote(PieceType::Bishop);
					}
					if (flag) {
						delete subGui;
						subGui = nullptr;
					}
					return;
				}
				// RESULT_NOTICE
				if (subGui->getGUIType() == GUIType::RESULT_NOTICE) {
					MatchResultGUI* temp = (MatchResultGUI*)subGui;
					if (checkFocus(e, temp->getRectOfBtnBackToMenu())) {
						std::cout << "Clicked menu button" << std::endl;
						soundManager->playClickBtnSound();
						delete subGui; 
						subGui = nullptr;
						delete mainGui;
						mainGui = new MenuGUI();
						soundManager->playMainMenuMusic();
						resetGame();
						return;
					}
					if (checkFocus(e, temp->getRectOfBtnSave())) {
						std::cout << "Clicked save button" << std::endl;
						soundManager->playClickBtnSound();
						saveCurrentGame("history.bin");
						return;
					}
					if (checkFocus(e, temp->getRectOfBtnPlayAgain())) {
						std::cout << "Clicked play again button" << std::endl;
						soundManager->playClickBtnSound();
						resetGame();
						state = GamePlayGUIState::PLAY;
						delete subGui;
						subGui = nullptr;
						return;
					}
				}
				// 
				if (subGui->getGUIType() == GUIType::SETTINGS) {
					SettingGUI* temp = (SettingGUI*)subGui;
					if (checkFocus(e, temp->getRectOfBtnBackToMenu())) {
						// Go to menu
						std::cout << "Clicked menu button" << std::endl;
						soundManager->playClickBtnSound();
						delete subGui;
						subGui = nullptr;
						delete mainGui;
						mainGui = new MenuGUI();
						soundManager->playMainMenuMusic();
						resetGame();
						return;
					}
					if (checkFocus(e, temp->getRectOfBtnResume())) {
						std::cout << "Clicked resume button" << std::endl;
						soundManager->playClickBtnSound();
						delete subGui;
						subGui = nullptr;
						return;
					}
					if (checkFocus(e, temp->getRectOfBtnSave())) {
						// Go to menu
						std::cout << "Clicked save button" << std::endl;
						soundManager->playClickBtnSound();
						saveCurrentGame("history.bin");
						return;
					}
					if (checkFocus(e, temp->getRectOfBtnVolume())) {
						// Go to menu
						std::cout << "Clicked volume button!" << std::endl;
						soundManager->playClickBtnSound();
						soundManager->muteXunmute();
						//loadPreviousGame("history.bin");
						// delete subGui; subGui = nullptr;
						return;
					}
				}
				return;
			}

			if (mainGui->getGUIType() == GUIType::MENU) {
				MenuGUI* temp = (MenuGUI*)mainGui;
				if (checkFocus(e, temp->getRectOfBtnVsPlayer())) {
					std::cout << "Clicked 2_player button!" << std::endl;
					delete mainGui;
					mainGui = new GamePlayGUI();
					opponent = Opponent::HUMAN;
					history->setOpponent(opponent);
					state = GamePlayGUIState::PLAY;
					soundManager->playClickBtnSound();
					soundManager->playPvPMusic();
					resetGame();
					return;
				}
				if (checkFocus(e, temp->getRectOfBtnVsComputerEasy())) {
					std::cout << "Clicked vs_easy_computer button!" << std::endl;
					soundManager->playClickBtnSound();
					delete mainGui;
					mainGui = new GamePlayGUI();
					opponent = Opponent::EASY_COMPUTER;
					history->setOpponent(opponent);
					state = GamePlayGUIState::PLAY;
					soundManager->playClickBtnSound();
					soundManager->playPvEEasyMusic();
					resetGame();
					return;
				}
				if (checkFocus(e, temp->getRectOfBtnVsComputerHard())) {
					std::cout << "Clicked vs_hard_computer button!" << std::endl;
					soundManager->playClickBtnSound();
					delete mainGui;
					mainGui = new GamePlayGUI();
					opponent = Opponent::HARD_COMPUTER;
					history->setOpponent(opponent);
					state = GamePlayGUIState::PLAY;
					soundManager->playPvEHardMusic();
					resetGame();
					return;
				}
				if (checkFocus(e, temp->getRectOfBtnContinueSavedGame())) {
					std::cout << "Clicked continue_saved_game button!" << std::endl;
					soundManager->playClickBtnSound();
					delete mainGui;
					mainGui = new GamePlayGUI();
					state = GamePlayGUIState::PLAY;
					soundManager->playPvPMusic();
					resetGame();
					history->read("history.bin");
					opponent = history->getOpponent();
					recoverGameFromHistory();
					return;
				}
				if (checkFocus(e, temp->getRectOfBtnReplayRecentGame())) {
					std::cout << "Clicked replay_recent_game button!" << std::endl;
					soundManager->playClickBtnSound();
					delete mainGui;
					mainGui = new GamePlayGUI();
					state = GamePlayGUIState::DISPLAY;
					soundManager->playReplayMusic();
					resetGame();
					history->read("history.bin");
					opponent = history->getOpponent();
					cnt = 0;
					return;
				}
				if (checkFocus(e, temp->getRectOfBtnVolume())) {
					std::cout << "Clicked volume button!" << std::endl;
					soundManager->playClickBtnSound();
					soundManager->muteXunmute();
					return;
				}
				if (checkFocus(e, temp->getRectOfBtnExit())) {
					std::cout << "Clicked exit button!" << std::endl;
					isRunning = false;
					return;
				}
				return;
			}

			if (mainGui->getGUIType() == GUIType::GAME_PLAY) {

				GamePlayGUI* temp = (GamePlayGUI*)mainGui;
				if (checkFocus(e, temp->getRectOfBtnSetting())) {
					std::cout << "Setting button clicked!" << std::endl;
					soundManager->playClickBtnSound();
					subGui = new SettingGUI();
					return;
				}

				if (state == GamePlayGUIState::DISPLAY) return;

				handleClickedPiece(e);
				handleClickedHighlightBox(e);

				if (checkFocus(e, temp->getRectOfBtnUndo())) {
					std::cout << "Undo button clicked!" << std::endl;
					soundManager->playClickBtnSound();
					undo();
					return;
				}
				if (checkFocus(e, temp->getRectOfBtnRedo())) {
					std::cout << "Redo button clicked!" << std::endl;
					soundManager->playClickBtnSound();
					redo();
					return;
				}
				return;
			}

		}
	}
}

Coordinate GameManager::getClickedBox(const SDL_Event& e) const {
	int x = e.motion.x;
	int y = e.motion.y;
	GamePlayGUI* gplay = dynamic_cast<GamePlayGUI*>(mainGui);
	if (!gplay) return Coordinate(-1, -1);
	SDL_Rect boardRect = gplay->getRectOfBoard();

	if (x < boardRect.x || y < boardRect.y || x > boardRect.x + boardRect.w || y > boardRect.y + boardRect.h) return Coordinate(-1, -1);

	double widthBox = boardRect.w / Board::XBOXES;
	double heightBox = boardRect.h / Board::YBOXES;

	Coordinate res;
	res.setX((x - boardRect.x) / widthBox);
	res.setY((y - boardRect.y) / heightBox);

	return res;
}

// TODO: (current default, white -> first: turn even, black: second -> turn odd;) make it flexible; add music;
void GameManager::handleClickedPiece(const SDL_Event& e) {	
	Coordinate c = getClickedBox(e);
	if (c.getX() < 0 && c.getY() < 0) return;
	Piece* piece = Board::getPieceAt(c);
	if (!piece) return;
	if (piece->getColor() == Color::White && turn % 2 == 1 || piece->getColor() == Color::Black && turn % 2 == 0) return;

	for (int i = 0; i < 32; i++) Board::piecesList[i]->setChosen(false);
	soundManager->playPieceMoveSound();

	piece->setChosen(true);
}

//TODO: add music
void GameManager::handleClickedHighlightBox(const SDL_Event& e) {
	std::vector<Coordinate> possibleMoves;
	Coordinate c = getClickedBox(e);
	if (c.getX() < 0 && c.getY() < 0) return;
	Piece* chosenPiece = nullptr;

	for (int i = 0; i < 32; i++) {
		if (Board::piecesList[i]->getChosen()) {
			chosenPiece = Board::piecesList[i];
			break;
		}
	}

	std::vector<std::vector<Coordinate>> temp;
	if (!chosenPiece) return;
	temp = chosenPiece->getPossibleMoves(Board::piecesOnBoard);
	possibleMoves.reserve(temp[0].size() + temp[1].size());
	std::copy(temp[0].begin(), temp[0].end(), std::back_inserter(possibleMoves));
	std::copy(temp[1].begin(), temp[1].end(), std::back_inserter(possibleMoves));

	for (auto& move : possibleMoves) {
		if (c == move) {
			Piece* capturedPiece = nullptr;
			history->setInitialState(chosenPiece);
			capturedPiece = chosenPiece->move(c, Board::piecesOnBoard);
			chosenPiece->setChosen(false);
			history->setFinalState(chosenPiece);
			history->setCapturedPiece(capturedPiece);
			history->updateData(turn);
			turn++;
			soundManager->playCapturingSound();
			Board::updateBoard();
			break;
		}
	}
	// TODO - check promotion
	if (checkPromotion(chosenPiece)) {
		std::cout << "Promotion!!!" << std::endl;
		subGui = new PromotionGUI();
	}

	chosenPiece = nullptr;
}

bool GameManager::checkFocus(const SDL_Event& e, const SDL_Rect& rect) const {
	int x = e.motion.x;
	int y = e.motion.y;

	if (x < rect.x || y < rect.y || x > rect.x + rect.w || y > rect.y + rect.h) return false;

	return true;
}

bool GameManager::checkPromotion(Piece* piece) {
	if (piece->getType() != PieceType::Pawn) return false;

	Coordinate c = piece->getPosition();
	if (c.getY() == 0 || c.getY() == 7) return true;

	return false;
}

void GameManager::promote(PieceType type) {
	int index = -1;
	for (int i = 0; i < 32; i++) {
		if (Board::piecesList[i]->getDead() || !checkPromotion(Board::piecesList[i])) continue;
		index = i;
		break;
	}

	if (index < 0 || index >= 32) return;
	Pawn* pawn = (Pawn*)Board::piecesList[index];
	switch (type) {
	case PieceType::Queen:
		Board::piecesList[index] = new Queen(pawn->getPosition(), pawn->getColor());
		Board::piecesList[index]->setID(pawn->getID());
		break;
	case PieceType::Rook:
		Board::piecesList[index] = new Rook(pawn->getPosition(), pawn->getColor());
		Board::piecesList[index]->setID(pawn->getID());
		((Rook*)Board::piecesList[index])->setFirstMove(false);
		break;
	case PieceType::Knight:
		Board::piecesList[index] = new Knight(pawn->getPosition(), pawn->getColor());
		Board::piecesList[index]->setID(pawn->getID());
		break;
	case PieceType::Bishop:
		Board::piecesList[index] = new Bishop(pawn->getPosition(), pawn->getColor());
		Board::piecesList[index]->setID(pawn->getID());
		break;
	}
	delete pawn;
	pawn = nullptr;

	// update history
	std::vector<Piece*> data = history->getData(turn - 1);
	history->setInitialState(data[0]);
	history->setFinalState(Board::piecesList[index]);
	history->setCapturedPiece(data[2]);
	history->updateData(turn - 1);
	// update board
	Board::updateBoard();
}

// TODO - Carefully pointer to texture (I call slow change state)
void GameManager::undo() {
	if (turn <= 0) return;

	turn--;
	std::vector<Piece*> temp = history->getData(turn);
	delete Board::piecesList[temp[0]->getID()];
	Board::piecesList[temp[0]->getID()] = temp[0]->clone();
	if (temp[2]) {
		delete Board::piecesList[temp[2]->getID()];
		Board::piecesList[temp[2]->getID()] = temp[2]->clone();
		Board::piecesList[temp[2]->getID()]->setDead(false);
	}

	// case: king performed castling
	if (temp[0]->getType() == PieceType::King && abs(temp[0]->getPosition().getX() - temp[1]->getPosition().getX()) == 2) {
		if (temp[0]->getPosition().getX() > temp[1]->getPosition().getX()) {
			Board::piecesList[temp[0]->getID() + 6]->setPosition(Coordinate(0, temp[0]->getPosition().getY()));
			((Rook*)(Board::piecesList[temp[0]->getID() + 6]))->setFirstMove(true);
		}
		else {
			Board::piecesList[temp[0]->getID() + 7]->setPosition(Coordinate(7, temp[0]->getPosition().getY()));
			((Rook*)(Board::piecesList[temp[0]->getID() + 7]))->setFirstMove(true);
		}
	}
	for (auto& e : Board::piecesList) e->setChosen(false);
	Board::updateBoard();

	std::cout << "------------------ start redo -----------------" << std::endl;
	std::cout << turn << std::endl;
	std::cout << "initial piece: " << history->getData(turn)[0] << std::endl;
	std::cout << "final piece: " << history->getData(turn)[1] << std::endl;
	std::cout << "captured piece: " << history->getData(turn)[2] << std::endl;
	std::cout << "------------------ end redo -----------------" << std::endl;

	if ((opponent == Opponent::EASY_COMPUTER || opponent == Opponent::HARD_COMPUTER) && turn % 2 == 0) {
		undo();
	}
}

// TODO - Carefully pointer to texture (I call quick change state)
void GameManager::redo() {
	if (turn >= history->getLengthData()) return;

	std::vector<Piece*> temp = history->getData(turn);
	delete Board::piecesList[temp[1]->getID()];
	Board::piecesList[temp[1]->getID()] = temp[1]->clone();
	if (temp[2]) {
		delete Board::piecesList[temp[2]->getID()];
		Board::piecesList[temp[2]->getID()] = temp[2]->clone();
	}

	// case: king performed castling
	if (temp[0]->getType() == PieceType::King && abs(temp[0]->getPosition().getX() - temp[1]->getPosition().getX()) == 2) {
		if (temp[0]->getPosition().getX() > temp[1]->getPosition().getX()) {
			Board::piecesList[temp[1]->getID() + 6]->setPosition(Coordinate(2, temp[1]->getPosition().getY()));
			((Rook*)(Board::piecesList[temp[1]->getID() + 6]))->setFirstMove(false);
		}
		else {
			Board::piecesList[temp[1]->getID() + 7]->setPosition(Coordinate(4, temp[1]->getPosition().getY()));
			((Rook*)(Board::piecesList[temp[1]->getID() + 7]))->setFirstMove(false);
		}
	}

	std::cout << "------------------ start undo -----------------" << std::endl;
	std::cout << turn << std::endl;
	std::cout << "initial piece: " << history->getData(turn)[0] << std::endl;
	std::cout << "final piece: " << history->getData(turn)[1] << std::endl;
	std::cout << "captured piece: " << history->getData(turn)[2] << std::endl;
	std::cout << "------------------ end undo -----------------" << std::endl;

	turn++;
	for (auto& e : Board::piecesList) e->setChosen(false);
	Board::updateBoard();

	if ((opponent == Opponent::EASY_COMPUTER || opponent == Opponent::HARD_COMPUTER) && turn % 2 == 0) {
		redo();
	}
}

void GameManager::resetGame() {
	turn = 0;
	Board::resetPiecesList();
	Board::updateBoard();
	history->clear();
}

void GameManager::saveCurrentGame(std::string path) {
	history->write(path);
}

void GameManager::loadPreviousGame(std::string path) {
	history->read(path);
	Board::resetPiecesList();
	Board::updateBoard();
	turn = 0;
}

MatchState GameManager::checkWinner() {
	if (turn % 2 == 0) {
		// this is turn of white pieces	
		King* king = (King*)(Board::piecesList[0]);
		bool kingChosen = king->getChosen();
		int cnt = 0;
		for (int i = 0; i < 16; i++) {
			if (Board::piecesList[i]->getDead()) continue;
			bool pieceChosen = Board::piecesList[i]->getChosen();
			Board::piecesList[i]->setChosen(true);
			std::vector<std::vector<Coordinate>> temp = Board::piecesList[i]->getPossibleMoves(Board::piecesOnBoard);
			cnt += temp[0].size() + temp[1].size();
			Board::piecesList[i]->setChosen(pieceChosen);
		}

		king->setChosen(true);
		if (cnt == 0) {
			if (king->checkmate(king->getPosition(), Board::piecesOnBoard)) return MatchState::BLACK_WIN;
			else return MatchState::DRAW;
		}
		king->setChosen(kingChosen);
	}
	else {
		// this is turn of black pieces	
		King* king = (King*)(Board::piecesList[16]);
		bool kingChosen = king->getChosen();
		int cnt = 0;
		for (int i = 16; i < 32; i++) {
			if (Board::piecesList[i]->getDead()) continue;
			bool pieceChosen = Board::piecesList[i]->getChosen();
			Board::piecesList[i]->setChosen(true);
			std::vector<std::vector<Coordinate>> temp = Board::piecesList[i]->getPossibleMoves(Board::piecesOnBoard);
			cnt += temp[0].size() + temp[1].size();
			Board::piecesList[i]->setChosen(pieceChosen);
		}

		king->setChosen(true);
		if (cnt == 0) {
			if (king->checkmate(king->getPosition(), Board::piecesOnBoard)) return MatchState::WHITE_WIN;
			else return MatchState::DRAW;
		}
		king->setChosen(kingChosen);
	}

	return MatchState::IN_PLAY;
}

void GameManager::recoverGameFromHistory() {
	Board::resetPiecesList();
	Board::updateBoard();
	turn = 0;

	for (int i = 0; i < history->getLengthData(); i++) {
		redo();
	}
}