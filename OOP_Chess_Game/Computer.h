#pragma once
#include<ctime>
#include "Pieces.h"
#include "Board.h"
#include <limits>


enum class Mode { EASY, HARD };

class Computer {
private:
	Mode mode;
public:
	Computer();
	// don't need destructor

	//void setMode(Mode mode);
	std::pair<int, Coordinate> playWithEasyMode();
	std::pair<int, Coordinate> evadeEaten();
	std::pair<int, Coordinate> getNonEatenMove(int idx);
	std::pair<int, Coordinate> playWithHardMode();
};
