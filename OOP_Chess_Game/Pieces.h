#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <SDL.h>
#include <SDL_image.h>
#include <fstream>

#include "Coordinate.h"
#include "Window.h"

#define _BOARD_HEIGHT 7
#define _BOARD_WIDTH 7

enum class Color {
	White,
	Black
};
enum class PieceType {
	King, Queen, Rook, Bishop, Knight, Pawn
};
enum class Path {
	queenWhite,
	queenBlack,
	kingWhite,
	kingBlack,
	rookWhite,
	rookBlack,
	knightWhite,
	knightBlack,
	bishopWhite,
	bishopBlack,
	pawnWhite,
	pawnBlack,
};

class Piece {
protected:
	Coordinate position;
	Color color;
	bool dead;
	bool chosen;
	PieceType type;
	int id;

	void updatePawnState(std::vector<std::vector<Piece*>> board);
	void getMovesWhenCheckmated(std::vector<Coordinate>& moves, std::vector<std::vector<Piece*>> board);
public:
	Piece();
	Piece(const Piece& piece);
	Piece(const Coordinate& position, Color color);
	virtual ~Piece();

	void setPosition(Coordinate postion);
	void setColor(Color color);
	void setDead(bool dead);
	void setChosen(bool chosen);
	void setID(int id);
	Coordinate getPosition() const;
	Color getColor() const;
	bool getDead() const;
	bool getChosen() const;
	int getID();
	PieceType getType() const;

	virtual Piece* move(const Coordinate& c, std::vector<std::vector<Piece*>>) = 0;
	virtual std::vector<std::vector<Coordinate>> getPossibleMoves(std::vector<std::vector<Piece*>>) = 0;
	virtual Piece* clone() const = 0;
	virtual void write(std::fstream& os);
	virtual void read(std::fstream& is);
};

class King : public Piece {
private:
	bool ableCastling;
public:
	King();
	King(const King& king);
	King(const Coordinate& position, Color color);
	virtual ~King();

	Piece* move(const Coordinate& c, std::vector<std::vector<Piece*>>);
	std::vector<std::vector<Coordinate>> getPossibleMoves(std::vector<std::vector<Piece*>>);
	Piece* clone() const;

	std::vector<Coordinate> getCastlingMove(std::vector<std::vector<Piece*>> board);
	bool checkmate(const Coordinate& positionOfKing, std::vector<std::vector<Piece*>> board);
	void write(std::fstream& os);
	void read(std::fstream& is);
};

class Queen : public Piece {
private:
public:
	Queen();
	Queen(const Queen& queen);
	Queen(const Coordinate& position, Color color);
	virtual ~Queen();

	Piece* move(const Coordinate& c, std::vector<std::vector<Piece*>>);
	std::vector<std::vector<Coordinate>> getPossibleMoves(std::vector<std::vector<Piece*>>);
	Piece* clone() const;
	void write(std::fstream& os);
	void read(std::fstream& is);
};

class Bishop : public Piece {
private:
public:
	Bishop();
	Bishop(const Bishop& queen);
	Bishop(const Coordinate& position, Color color);
	virtual ~Bishop();

	Piece* move(const Coordinate& c, std::vector<std::vector<Piece*>>);
	std::vector<std::vector<Coordinate>> getPossibleMoves(std::vector<std::vector<Piece*>>);
	Piece* clone() const;
	void write(std::fstream& os);
	void read(std::fstream& is);
};

class Rook : public Piece {
private:
	bool firstMove;
public:
	Rook();
	Rook(const Rook& rook);
	Rook(const Coordinate& position, Color color);
	virtual ~Rook();

	void setFirstMove(bool firstMove);
	bool getFirstMove();


	Piece* move(const Coordinate& c, std::vector<std::vector<Piece*>>);
	std::vector<std::vector<Coordinate>> getPossibleMoves(std::vector<std::vector<Piece*>>);
	Piece* clone() const;
	void write(std::fstream& os);
	void read(std::fstream& is);
};

class Knight : public Piece {
private:
public:
	Knight();
	Knight(const Knight& knight);
	Knight(const Coordinate& position, Color color);
	virtual ~Knight();

	Piece* move(const Coordinate& c, std::vector<std::vector<Piece*>>);
	std::vector<std::vector<Coordinate>> getPossibleMoves(std::vector<std::vector<Piece*>>);
	Piece* clone() const;
	void write(std::fstream& os);
	void read(std::fstream& is);
};

class Pawn: public Piece {
private:
	bool firstMove;
	bool enableEnPassantCaptured;
public:
	Pawn();
	Pawn(const Pawn& pawn);
	Pawn(const Coordinate& position, Color color);
	virtual ~Pawn();

	bool getFirstMove();
	void setFirstMove(bool firstMove);
	void setEnableEnPassantCaptured(bool able);
	bool getEnableEnPassantCaptured();
	Piece* move(const Coordinate& c, std::vector<std::vector<Piece*>>);
	std::vector<std::vector<Coordinate>> getPossibleMoves(std::vector<std::vector<Piece*>>);
	Piece* clone() const;
	void write(std::fstream& os);
	void read(std::fstream& is);
};

