#include "Pieces.h"

Piece::Piece() {
	Coordinate::Coordinate();
	color = {};
	dead = false;
	chosen = false; 
	type = {};
	id = -1;
}
Piece::Piece(const Piece& piece) {
	this->position = piece.position;
	this->color = piece.color;
	this->dead = piece.dead;
	this->chosen = piece.chosen;
	this->type = piece.type;
	this->id = piece.id;
}
Piece::Piece(const Coordinate& position, Color color) {
	this->position = position;
	this->color = color;
}
Piece::~Piece() {
}
void Piece::updatePawnState(std::vector<std::vector<Piece*>> board) {
	for (auto& u : board) {
		for (auto& v : u) {
			if (v && this->color == v->getColor() && v->getType() == PieceType::Pawn) {
				Pawn* pawn = (Pawn*)v;
				pawn->setEnableEnPassantCaptured(false);
			}
		}
	}
}

void Piece::setPosition(Coordinate position) {
	this->position = position;
}
void Piece::setColor(Color color) {
	this->color = color;
}
void Piece::setDead(bool dead) {
	this->dead = dead;
}
void Piece::setChosen(bool chosen) {
	this->chosen = chosen;
}
void Piece::setID(int id) {
	this->id = id;
}

Coordinate Piece::getPosition() const {
	return this->position;
}
Color Piece::getColor() const {
	return this->color;
}
bool Piece::getDead() const {
	return this->dead;
}
bool Piece::getChosen() const {
	return this->chosen;
}
PieceType Piece::getType() const {
	return this->type;
}
int Piece::getID() {
	return this->id;
}

void Piece::getMovesWhenCheckmated(std::vector<Coordinate>& moves, std::vector<std::vector<Piece*>> board) {
	if (this->getChosen()) {
		King* king = nullptr;
		for (auto& v : board) {
			bool flag = false;
			for (auto& u : v) {
				if (u && u->getColor() == this->color && u->getType() == PieceType::King) {
					king = (King*)u;
					flag = true;
				}
			}
			if (flag) break;
		}
		for (int i = 0; i < moves.size(); i++) {
			Piece* temp = nullptr;
			Coordinate move = moves[i];
			temp = board[moves[i].getX()][moves[i].getY()];
			board[this->position.getX()][this->position.getY()] = nullptr;
			board[moves[i].getX()][moves[i].getY()] = this;
			if (king && king->checkmate(king->getPosition(), board)) {
				moves.erase(moves.begin() + i);
				i--;
			}
			board[move.getX()][move.getY()] = temp;
			board[this->position.getX()][this->position.getY()] = this;
		}
	}
}

void Piece::write(std::fstream& os) {
	os.write((char*)&position, sizeof(position));
	os.write((char*)&color, sizeof(color));
	os.write((char*)&dead, sizeof(color));
	os.write((char*)&chosen, sizeof(chosen));
	os.write((char*)&type, sizeof(type));
	os.write((char*)&id, sizeof(id));
}

void Piece::read(std::fstream& is) {
	is.read((char*)&position, sizeof(position));
	is.read((char*)&color, sizeof(color));
	is.read((char*)&dead, sizeof(color));
	is.read((char*)&chosen, sizeof(chosen));
	is.read((char*)&type, sizeof(type));
	is.read((char*)&id, sizeof(id));
}

//--------------------------------------------------------------------------------------------------
King::King() {
	ableCastling = true;
}
King::King(const King& king) : Piece(king) {
	this->type = PieceType::King;
	this->ableCastling = king.ableCastling;
}
King::King(const Coordinate& position, Color color) : Piece(position, color) {
	this->type = PieceType::King;
	this->dead = false;
	this->chosen = false;
	this->ableCastling = true;
}
King::~King() {

}
Piece* King::move(const Coordinate& c, std::vector<std::vector<Piece*>> board) {
	if (ableCastling && this->chosen) {
		std::vector<Coordinate> moves = getCastlingMove(board);
		if (moves.size() != 0) {
			for (auto& pos : moves) {
				if (pos == c) {
					if (this->position.getX() > c.getX() && board[0][c.getY()]) {
						board[0][c.getY()]->move(Coordinate(2, c.getY()), board);
					}
					else if (this->position.getX() < c.getX() && board[7][c.getY()]) {
						board[7][c.getY()]->move(Coordinate(4, c.getY()), board);
					}
					this->updatePawnState(board);
					this->setPosition(c);
					ableCastling = false;

					return nullptr;
				}
			}
		}
		ableCastling = false;
	}

	if (!board[c.getX()][c.getY()]) {
		this->updatePawnState(board);
		this->setPosition(c);

		return nullptr;
	}

	if (this->getColor() != board[c.getX()][c.getY()]->getColor()) {
		this->updatePawnState(board);
		this->setPosition(c);
		board[c.getX()][c.getY()]->setDead(true);

		return board[c.getX()][c.getY()];
	}
	return nullptr;
}
std::vector<std::vector<Coordinate>> King::getPossibleMoves(std::vector<std::vector<Piece*>> board) {
	std::vector<Coordinate> moves;
	std::vector<std::vector<Coordinate>> result;
	result.resize(2);
	Coordinate tmp(this->getPosition().getX(), this->getPosition().getY());
	if (ableCastling && this->getChosen()) {
		std::vector<Coordinate> tmpMoves = this->getCastlingMove(board);
		for (auto& i : tmpMoves) {
			moves.push_back(i);
		}
	}
	//x - 1, y - 1
	int X = tmp.getX() - 1;
	int Y = tmp.getY() - 1;
	if (X >= 0 && Y >= 0) {
		if (!board[X][Y] || this->getColor() != board[X][Y]->getColor()) {
			moves.push_back(Coordinate(X, Y));
		}
	}
	//x + 1, y - 1
	X = tmp.getX() + 1;
	Y = tmp.getY() - 1;
	if (X <= _BOARD_HEIGHT && Y >= 0) {
		if (!board[X][Y] || this->getColor() != board[X][Y]->getColor()) {
			moves.push_back(Coordinate(X, Y));
		}
	}
	//x - 1, y + 1
	X = tmp.getX() - 1;
	Y = tmp.getY() + 1;
	if (X >= 0 && Y <= _BOARD_WIDTH) {
		if (!board[X][Y] || this->getColor() != board[X][Y]->getColor()) {
			moves.push_back(Coordinate(X, Y));
		}
	}
	//x + 1, y + 1
	X = tmp.getX() + 1;
	Y = tmp.getY() + 1;
	if (X <= _BOARD_HEIGHT && Y <= _BOARD_WIDTH) {
		if (!board[X][Y] || this->getColor() != board[X][Y]->getColor()) {
			moves.push_back(Coordinate(X, Y));
		}
	}
	//x + 1
	X = tmp.getX() + 1;
	Y = tmp.getY();
	if (X <= _BOARD_HEIGHT) {
		if (!board[X][Y] || this->getColor() != board[X][Y]->getColor()) {
			moves.push_back(Coordinate(X, Y));
		}
	}
	//x - 1
	X = tmp.getX() - 1;
	Y = tmp.getY();
	if (X >= 0) {
		if (!board[X][Y] || this->getColor() != board[X][Y]->getColor()) {
			moves.push_back(Coordinate(X, Y));
		}
	}
	//y + 1
	X = tmp.getX();
	Y = tmp.getY() + 1;
	if (Y <= _BOARD_WIDTH) {
		if (!board[X][Y] || this->getColor() != board[X][Y]->getColor()) {
			moves.push_back(Coordinate(X, Y));
		}
	}
	//y - 1
	X = tmp.getX();
	Y = tmp.getY() - 1;
	if (Y >= 0) {
		if (!board[X][Y] || this->getColor() != board[X][Y]->getColor()) {
			moves.push_back(Coordinate(X, Y));
		}
	}

	// case: checkmate
	if (this->getChosen()) {
		for (int i = 0; i < moves.size(); i++) {
			Coordinate move = moves[i];
			Piece* temp = board[move.getX()][move.getY()];
			board[move.getX()][move.getY()] = this;
			board[this->position.getX()][this->position.getY()] = nullptr;
			if (checkmate(moves[i], board)) {
				moves.erase(moves.begin() + i);
				i--;
			}
			board[move.getX()][move.getY()] = temp;
			board[this->position.getX()][this->position.getY()] = this;
		}
	}

	for (auto& e : moves) {
		if (!board[e.getX()][e.getY()]) {
			result[0].push_back(e);
		}
		else result[1].push_back(e);
	}

	return result;
}
Piece* King::clone() const{
	return new King(*this);
}
std::vector<Coordinate> King::getCastlingMove(std::vector<std::vector<Piece*>> board) {
	/*
	Rule 1. You cannot castle if you have moved your king(or the rook)!
	Rule 2. You are not allowed to castle out of check!
	Rule 3. You are not allowed to castle through check!
	Rule 4. No pieces can be between the king and the rook
	*/
	std::vector<Coordinate> moves;
	std::vector<Rook*> rooks;
	for (int i = 0; i <= 7; i += 7) {
		Rook* temp = dynamic_cast<Rook*>(board[i][this->position.getY()]);
		rooks.push_back(temp);
	}

	// Violate Rule 1
	if (!ableCastling || !rooks[0] && !rooks[1]) {
		ableCastling = false; 
		return moves;
	}
	// Violate Rule 2
	if (checkmate(this->position, board)) return moves;

	for (int i = 0; i < 2; i++) {
		// Violate rule 1
		if (!rooks[i]) continue;
		if (!rooks[i]->getFirstMove()) continue;

		bool flag = false;
		int coeff = pow(-1, i + 1);
		int tempY = this->position.getY(), tempX = this->position.getX();
		while (true) {
			tempX = tempX + coeff;
			if (tempX <= 0 || tempX >= 7) break;

			if (board[tempX][tempY]) {
				flag = true;
				break;
			}
		}
		// Violate rule 4
		if (flag) continue;

		// Violate rule 3
		if (checkmate(Coordinate(this->position.getX() + coeff * 2, this->position.getY()), board)) continue;

		moves.push_back(Coordinate(this->position.getX() + coeff * 2, this->position.getY()));
	}

	return moves;
}
bool King::checkmate(const Coordinate& positionOfKing, std::vector<std::vector<Piece*>> board) {
	std::vector<Coordinate> possibleMoves;

	for (auto& row : board) {
		for (Piece* piece : row) {
			if (!piece) {
				continue;
			}
			
			if (this->getColor() != piece->getColor()) {
				std::vector<std::vector<Coordinate>> temp = piece->getPossibleMoves(board);
				possibleMoves.reserve(temp[0].size() + temp[1].size());
				std::copy(temp[0].begin(), temp[0].end(), std::back_inserter(possibleMoves));
				std::copy(temp[1].begin(), temp[1].end(), std::back_inserter(possibleMoves));

			}
		}
	}
	for (auto& c : possibleMoves) {
		if (c.getX() == positionOfKing.getX() && c.getY() == positionOfKing.getY()) {
			return true;
		}
	}
	return false;
}
void King::write(std::fstream& os) {
	Piece::write(os);
	os.write((char*)&ableCastling, sizeof(ableCastling));
}
void King::read(std::fstream& is) {
	Piece::read(is);
	is.read((char*)&ableCastling, sizeof(ableCastling));
}

//--------------------------------------------------------------------------------------------------
Queen::Queen() {

}
Queen::Queen(const Queen& queen) : Piece(queen) {
	this->type = PieceType::Queen;
}
Queen::Queen(const Coordinate& position, Color color) : Piece(position, color) {
	this->dead = false;
	this->chosen = false;
	this->type = PieceType::Queen;
}
Queen::~Queen() {

}

Piece* Queen::move(const Coordinate& c, std::vector<std::vector<Piece*>> board) {
	if (!board[c.getX()][c.getY()]) {
		this->updatePawnState(board);
		this->setPosition(c);

		return nullptr;
	}
	
	if (this->getColor() != board[c.getX()][c.getY()]->getColor()) {
		this->updatePawnState(board);
		this->setPosition(c);
		board[c.getX()][c.getY()]->setDead(true);

		return board[c.getX()][c.getY()];
	}
	return nullptr;
}
std::vector<std::vector<Coordinate>> Queen::getPossibleMoves(std::vector<std::vector<Piece*>> board) {
	std::vector<Coordinate> moves;
	std::vector<std::vector<Coordinate>> result;
	result.resize(2);
	Coordinate tmp(this->getPosition().getX(), this->getPosition().getY());
	//x - 1
	int X = tmp.getX() - 1;
	int Y = tmp.getY();
	while (X >= 0) {
		if (!board[X][Y]) {
			moves.push_back(Coordinate(X, Y));
			X--;
		}
		else if (this->getColor() !=  board[X][Y]->getColor()) { 
			moves.push_back(Coordinate(X, Y));
			break; 
		}
		else { break; }
	}

	//x + 1
	X = tmp.getX() + 1;
	Y = tmp.getY();
	while (X <= _BOARD_HEIGHT) {
		if (!board[X][Y]) {
			moves.push_back(Coordinate(X, Y));
			X++;
		}
		else if (this->getColor() != board[X][Y]->getColor()) {
			moves.push_back(Coordinate(X, Y));
			break;
		}
		else { break; }
	}
	
	//y - 1
	X = tmp.getX();
	Y = tmp.getY() - 1;
	while (Y >= 0) {
		if (!board[X][Y]) {
			moves.push_back(Coordinate(X, Y));
			Y--;
		}
		else if (this->getColor() != board[X][Y]->getColor()) {
			moves.push_back(Coordinate(X, Y));
			break;
		}
		else { break; }
	}

	//y + 1
	X = tmp.getX();
	Y = tmp.getY() + 1;
	while (Y <= _BOARD_WIDTH) {
		if (!board[X][Y]) {
			moves.push_back(Coordinate(X, Y));
			Y++;
		}
		else if (this->getColor() != board[X][Y]->getColor()) {
			moves.push_back(Coordinate(X, Y));
			break;
		}
		else { break; }
	}

	//x + 1, y + 1
	X = tmp.getX() + 1;
	Y = tmp.getY() + 1;
	while (X <= _BOARD_HEIGHT && Y <= _BOARD_WIDTH) {
		if (!board[X][Y]) {
			moves.push_back(Coordinate(X, Y));
			X++;
			Y++;
		}
		else if (this->getColor() != board[X][Y]->getColor()) {
			moves.push_back(Coordinate(X, Y));
			break;
		}
		else { break; }
	}

	//x - 1, y - 1
	X = tmp.getX() - 1;
	Y = tmp.getY() - 1;
	while (X >= 0 && Y >= 0) {
		if (!board[X][Y]) {
			moves.push_back(Coordinate(X, Y));
			X--;
			Y--;
		}
		else if (this->getColor() != board[X][Y]->getColor()) {
			moves.push_back(Coordinate(X, Y));
			break;
		}
		else { break; }
	}

	//x - 1, y + 1
	X = tmp.getX() - 1;
	Y = tmp.getY() + 1;	
	while (X >= 0 && Y <= _BOARD_WIDTH) {
		if (!board[X][Y]) {
			moves.push_back(Coordinate(X, Y));
			X--;
			Y++;
		}
		else if (this->getColor() != board[X][Y]->getColor()) {
			moves.push_back(Coordinate(X, Y));
			break;
		}
		else { break; }
	}

	//x + 1, y - 1
	X = tmp.getX() + 1;
	Y = tmp.getY() - 1;	
	while (X <= _BOARD_HEIGHT && Y >= 0) {
		if (!board[X][Y]) {
			moves.push_back(Coordinate(X, Y));
			X++;
			Y--;
		}
		else if (this->getColor() != board[X][Y]->getColor()) {
			moves.push_back(Coordinate(X, Y));
			break;
		}
		else { break; }
	}

	// case: checkmate
	this->getMovesWhenCheckmated(moves, board);

	for (auto& e : moves) {
		if (!board[e.getX()][e.getY()]) result[0].push_back(e);
		else result[1].push_back(e);
	}

	return result;
}
Piece* Queen::clone() const{
	return new Queen(*this);
}
void Queen::write(std::fstream& os) {
	Piece::write(os);
}
void Queen::read(std::fstream& is) {
	Piece::read(is);
}

//--------------------------------------------------------------------------------
Bishop::Bishop() {

}
Bishop::Bishop(const Bishop& bishop) : Piece(bishop) {
	this->type = PieceType::Bishop;
}
Bishop::Bishop(const Coordinate& position, Color color) : Piece(position, color) {
	this->dead = false;
	this->chosen = false;
	this->type = PieceType::Bishop;
}
Bishop::~Bishop() {

}

Piece* Bishop::move(const Coordinate& c, std::vector<std::vector<Piece*>> board) {
	if (!board[c.getX()][c.getY()]) {
		this->updatePawnState(board);
		this->setPosition(c);

		return nullptr;
	}
	if (this->getColor() != board[c.getX()][c.getY()]->getColor()) {
		this->updatePawnState(board);
		this->setPosition(c);
		board[c.getX()][c.getY()]->setDead(true);

		return board[c.getX()][c.getY()];
	}

	return nullptr;
}
std::vector<std::vector<Coordinate>> Bishop::getPossibleMoves(std::vector<std::vector<Piece*>> board) {
	std::vector<Coordinate> moves;
	std::vector<std::vector<Coordinate>> result;
	result.resize(2);
	Coordinate tmp(this->getPosition().getX(), this->getPosition().getY());

	//x + 1, y + 1
	int X = tmp.getX() + 1;
	int Y = tmp.getY() + 1;
	while (X <= _BOARD_HEIGHT && Y <= _BOARD_WIDTH) {
		if (!board[X][Y]) {
			moves.push_back(Coordinate(X, Y));
			X++;
			Y++;
		}
		else if (this->getColor() != board[X][Y]->getColor()) {
			moves.push_back(Coordinate(X, Y));
			break;
		}
		else { break; }
	}

	//x - 1, y - 1
	X = tmp.getX() - 1;
	Y = tmp.getY() - 1;
	while (X >= 0 && Y >= 0) {
		if (!board[X][Y]) {
			moves.push_back(Coordinate(X, Y));
			X--;
			Y--;
		}
		else if (this->getColor() != board[X][Y]->getColor()) {
			moves.push_back(Coordinate(X, Y));
			break;
		}
		else { break; }
	}

	//x - 1, y + 1
	X = tmp.getX() - 1;
	Y = tmp.getY() + 1;
	while (X >= 0 && Y <= _BOARD_WIDTH) {
		if (!board[X][Y]) {
			moves.push_back(Coordinate(X, Y));
			X--;
			Y++;
		}
		else if (this->getColor() != board[X][Y]->getColor()) {
			moves.push_back(Coordinate(X, Y));
			break;
		}
		else { break; }
	}

	//x + 1, y - 1
	X = tmp.getX() + 1;
	Y = tmp.getY() - 1;
	while (X <= _BOARD_HEIGHT && Y >= 0) {
		if (!board[X][Y]) {
			moves.push_back(Coordinate(X, Y));
			X++;
			Y--;
		}
		else if (this->getColor() != board[X][Y]->getColor()) {
			moves.push_back(Coordinate(X, Y));
			break;
		}
		else { break; }
	}

	// case: checkmate
	this->getMovesWhenCheckmated(moves, board);

	for (auto& e : moves) {
		if (!board[e.getX()][e.getY()]) result[0].push_back(e);
		else result[1].push_back(e);
	}

	return result;
}
Piece* Bishop::clone() const{
	return new Bishop(*this);
}
void Bishop::write(std::fstream& os) {
	Piece::write(os);
}
void Bishop::read(std::fstream& is) {
	Piece::read(is);
}

//----------------------------------------------------------------------------------
Rook::Rook() {
	firstMove = true;
}
Rook::Rook(const Rook& rook) : Piece(rook) {
	this->type = PieceType::Rook;
	this->firstMove = rook.firstMove;
}
Rook::Rook(const Coordinate& position, Color color) : Piece(position, color) {
	this->dead = false;
	this->chosen = false;
	this->type = PieceType::Rook;
	this->firstMove = true;
}
Rook::~Rook() {

}
void Rook::setFirstMove(bool firstMove) {
	this->firstMove = firstMove;
}
bool Rook::getFirstMove() {
	return firstMove;
}
Piece* Rook::move(const Coordinate& c, std::vector<std::vector<Piece*>> board) {
	if (firstMove) {
		firstMove = false;
	}
	if (!board[c.getX()][c.getY()]) {
		this->updatePawnState(board);
		this->setPosition(c);

		return nullptr;
	}
	if (this->getColor() != board[c.getX()][c.getY()]->getColor()) {
		this->updatePawnState(board);
		this->setPosition(c);
		board[c.getX()][c.getY()]->setDead(true);

		return board[c.getX()][c.getY()];
	}
	return nullptr;
}
std::vector<std::vector<Coordinate>> Rook::getPossibleMoves(std::vector<std::vector<Piece*>> board) {
	std::vector<Coordinate> moves;
	std::vector<std::vector<Coordinate>> result;
	result.resize(2);
	Coordinate tmp(this->getPosition().getX(), this->getPosition().getY());

	//x - 1
	int X = tmp.getX() - 1;
	int Y = tmp.getY();
	while (X >= 0) {
		if (!board[X][Y]) {
			moves.push_back(Coordinate(X, Y));
			X--;
		}
		else if (this->getColor() != board[X][Y]->getColor()) {
			moves.push_back(Coordinate(X, Y));
			break;
		}
		else { break; }
	}

	//x + 1
	X = tmp.getX() + 1;
	Y = tmp.getY();
	while (X <= _BOARD_HEIGHT) {
		if (!board[X][Y]) {
			moves.push_back(Coordinate(X, Y));
			X++;
		}
		else if (this->getColor() != board[X][Y]->getColor()) {
			moves.push_back(Coordinate(X, Y));
			break;
		}
		else { break; }
	}

	//y - 1
	X = tmp.getX();
	Y = tmp.getY() - 1;	
	while (Y >= 0) {
		if (!board[X][Y]) {
			moves.push_back(Coordinate(X, Y));
			Y--;
		}
		else if (this->getColor() != board[X][Y]->getColor()) {
			moves.push_back(Coordinate(X, Y));
			break;
		}
		else { break; }
	}

	//y + 1
	X = tmp.getX();
	Y = tmp.getY() + 1;
	while (Y <= _BOARD_WIDTH) {
		if (!board[X][Y]) {
			moves.push_back(Coordinate(X, Y));
			Y++;
		}
		else if (this->getColor() != board[X][Y]->getColor()) {
			moves.push_back(Coordinate(X, Y));
			break;
		}
		else { break; }
	}

	// case: checkmate
	this->getMovesWhenCheckmated(moves, board);

	for (auto& e : moves) {
		if (!board[e.getX()][e.getY()]) result[0].push_back(e);
		else result[1].push_back(e);
	}

	return result;
}
Piece* Rook::clone() const{
	return new Rook(*this);
}
void Rook::write(std::fstream& os) {
	Piece::write(os);
	os.write((char*)&firstMove, sizeof(firstMove));
}
void Rook::read(std::fstream& is){
	Piece::read(is);
	is.read((char*)&firstMove, sizeof(firstMove));
}

//Rook& Rook::operator=(const Rook& piece) {
//	if (this == &piece) return *this;
//
//	this->position = piece.position;
//	this->color = piece.color;
//	this->dead = piece.dead;
//	this->chosen = piece.chosen;
//	this->imagePath = piece.imagePath;
//	this->texture = nullptr;
//	this->type = piece.type;
//	this->firstMove = piece.firstMove;
//	this->id = piece.id;
//
//	return *this;
//}
//------------------------------------------------------------------------------
Knight::Knight() {

}
Knight::Knight(const Knight& knight) : Piece(knight) {
	this->type = PieceType::Knight;
}
Knight::Knight(const Coordinate& position, Color color) : Piece(position, color) {
	this->dead = false;
	this->chosen = false;
	this->type = PieceType::Knight;
}
Knight::~Knight() {

}

Piece* Knight::move(const Coordinate& c, std::vector<std::vector<Piece*>> board) {
	if (!board[c.getX()][c.getY()]) {
		this->updatePawnState(board);
		this->setPosition(c);

		return nullptr;
	}
	if (this->getColor() != board[c.getX()][c.getY()]->getColor()) {
		this->updatePawnState(board);
		this->setPosition(c);
		board[c.getX()][c.getY()]->setDead(true);

		return board[c.getX()][c.getY()];
	}
	return nullptr;
}
std::vector<std::vector<Coordinate>> Knight::getPossibleMoves(std::vector<std::vector<Piece*>> board) {
	std::vector<Coordinate> moves;
	std::vector<std::vector<Coordinate>> result;
	result.resize(2);
	Coordinate tmp(this->getPosition().getX(), this->getPosition().getY());

	//x + 1, y - 2
	int X = tmp.getX() + 1;
	int Y = tmp.getY() - 2;
	if (X <= _BOARD_HEIGHT && Y >= 0) {
		if (!board[X][Y] || this->getColor() != board[X][Y]->getColor()) {
			moves.push_back(Coordinate(X, Y));
		}
	}

	//x + 1, y + 2
	X = tmp.getX() + 1;
	Y = tmp.getY() + 2;
	if (X <= _BOARD_HEIGHT && Y <= _BOARD_WIDTH) {
		if (!board[X][Y] || this->getColor() != board[X][Y]->getColor()) {
			moves.push_back(Coordinate(X, Y));
		}
	}

	//x - 1, y + 2
	X = tmp.getX() - 1;
	Y = tmp.getY() + 2;

	if (X >= 0 && Y <= _BOARD_WIDTH) {
		if (!board[X][Y] || this->getColor() != board[X][Y]->getColor()) {
			moves.push_back(Coordinate(X, Y));
		}
	}

	//x - 1, y - 2
	X = tmp.getX() - 1;
	Y = tmp.getY() - 2;
	if (X >= 0 && Y >= 0) {
		if (!board[X][Y] || this->getColor() != board[X][Y]->getColor()) {
			moves.push_back(Coordinate(X, Y));
		}
	}

	//x + 2, y + 1
	X = tmp.getX() + 2;
	Y = tmp.getY() + 1;

	if (X <= _BOARD_HEIGHT && Y <= _BOARD_WIDTH) {
		if (!board[X][Y] || this->getColor() != board[X][Y]->getColor()) {
			moves.push_back(Coordinate(X, Y));
		}
	}

	//x - 2, y + 1
	X = tmp.getX() - 2;
	Y = tmp.getY() + 1;
	if (X >= 0 && Y <= _BOARD_WIDTH) {
		if (!board[X][Y] || this->getColor() != board[X][Y]->getColor()) {
			moves.push_back(Coordinate(X, Y));
		}
	}
	//x + 2, y - 1
	X = tmp.getX() + 2;
	Y = tmp.getY() - 1;
	if (X <= _BOARD_HEIGHT && Y >= 0) {
		if (!board[X][Y] || this->getColor() != board[X][Y]->getColor()) {
			moves.push_back(Coordinate(X, Y));
		}
	}
	//x - 2, y - 1
	X = tmp.getX() - 2;
	Y = tmp.getY() - 1;

	if (X >= 0 && Y >= 0) {
		if (!board[X][Y] || this->getColor() != board[X][Y]->getColor()) {
			moves.push_back(Coordinate(X, Y));
		}
	}

	// case: checkmate
	this->getMovesWhenCheckmated(moves, board);

	for (auto& e : moves) {
		if (!board[e.getX()][e.getY()]) result[0].push_back(e);
		else result[1].push_back(e);
	}

	return result;
}
Piece* Knight::clone() const{
	return new Knight(*this);
}
void Knight::write(std::fstream& os) {
	Piece::write(os);
}
void Knight::read(std::fstream& is) {
	Piece::read(is);
}

//---------------------------------------------------------------------------
Pawn::Pawn() {
	firstMove = true;
	enableEnPassantCaptured = false;
}
Pawn::Pawn(const Pawn& pawn) : Piece(pawn) {
	this->type = PieceType::Pawn;
	this->firstMove = pawn.firstMove;
	this->enableEnPassantCaptured = pawn.enableEnPassantCaptured;
}
Pawn::Pawn(const Coordinate& position, Color color) : Piece(position, color) {
	this->type = PieceType::Pawn;
	this->dead = false;
	this->chosen = false;
	firstMove = true;
	enableEnPassantCaptured = false;
}
Pawn::~Pawn() {
}
void Pawn::setFirstMove(bool firstMove) {
	this->firstMove = firstMove;
}
void Pawn::setEnableEnPassantCaptured(bool able) {
	this->enableEnPassantCaptured = able;
}
bool Pawn::getEnableEnPassantCaptured() {
	return this->enableEnPassantCaptured;
}
bool Pawn::getFirstMove() {
	return this->firstMove;
}
Piece* Pawn::move(const Coordinate& c, std::vector<std::vector<Piece*>> board) {
	if (this->getFirstMove()) {
		this->updatePawnState(board);
		if (!board[c.getX()][c.getY()]) {
			if ((this->color == Color::White && c.getY() == 3) || (this->color == Color::Black && c.getY() == 4)) {
				this->setEnableEnPassantCaptured(true);
			}
			this->setPosition(c);
			this->setFirstMove(false);

			return nullptr;
		}
		if (board[c.getX()][c.getY()] != nullptr && this->getColor() != board[c.getX()][c.getY()]->getColor()) {
			board[c.getX()][c.getY()]->setDead(true);
			this->setPosition(c);
			this->setFirstMove(false);

			return board[c.getX()][c.getY()];
		}
	}
	Coordinate tmp = this->getPosition();
	int X = tmp.getX();
	int Y = tmp.getY();
	if (this->getColor() == Color::White) {
		if (X > 0 && Y == 4 && X - 1 == c.getX() && c.getY() == 5 && board[X - 1][Y] && board[X - 1][Y]->getType() == PieceType::Pawn) {
			Pawn* enemyPawn = (Pawn*)board[X - 1][Y];
			if (enemyPawn && enemyPawn->getColor() == Color::Black && enemyPawn->getEnableEnPassantCaptured()) {
				board[X - 1][Y]->setDead(true);
				this->setPosition(c);

				return board[X - 1][Y];
			}
		}
		if (X < _BOARD_HEIGHT && Y == 4 && X + 1 == c.getX() && c.getY() == 5 && board[X + 1][Y] && board[X + 1][Y]->getType() == PieceType::Pawn) {
			Pawn* enemyPawn = (Pawn*)board[X + 1][Y];
			if (enemyPawn && enemyPawn->getColor() == Color::Black && enemyPawn->getEnableEnPassantCaptured()) {
				board[X + 1][Y]->setDead(true);
				this->setPosition(c);

				return board[X + 1][Y];
			}
		}
	}
	if (this->getColor() == Color::Black) {
		if (X > 0 && Y == 3 && X - 1 == c.getX() && c.getY() == 2 && board[X - 1][Y] && board[X - 1][Y]->getType() == PieceType::Pawn) {
			Pawn* enemyPawn = (Pawn*)board[X - 1][Y];
			if (enemyPawn && enemyPawn->getColor() == Color::White && enemyPawn->getEnableEnPassantCaptured()) {
				board[X - 1][Y]->setDead(true);
				this->setPosition(c);

				return board[X - 1][Y];
			}
		}
		if (X < _BOARD_HEIGHT && Y == 3 && X + 1 == c.getX() && c.getY() == 2 && board[X + 1][Y] && board[X + 1][Y]->getType() == PieceType::Pawn) {
			Pawn* enemyPawn = (Pawn*)board[X + 1][Y];
			if (enemyPawn && enemyPawn->getColor() == Color::White && enemyPawn->getEnableEnPassantCaptured()) {
				board[X + 1][Y]->setDead(true);
				this->setPosition(c);

				return board[X + 1][Y];
			}
		}
	}
	if (!board[c.getX()][c.getY()]) {
		this->updatePawnState(board);
		this->setPosition(c);

		return nullptr;
	}
	if (this->getColor() != board[c.getX()][c.getY()]->getColor()) {
		this->updatePawnState(board);
		board[c.getX()][c.getY()]->setDead(true);
		this->setPosition(c);

		return board[c.getX()][c.getY()];
	}
	return nullptr;
}
std::vector<std::vector<Coordinate>> Pawn::getPossibleMoves(std::vector<std::vector<Piece*>> board) {
	std::vector<Coordinate> moves;
	std::vector<std::vector<Coordinate>> result;
	result.resize(2);
	Coordinate tmp(this->getPosition().getX(), this->getPosition().getY());
	int X = tmp.getX();
	int Y = tmp.getY();

	if (this->getColor() == Color::White) {
		if (Y == _BOARD_WIDTH) {
			return result;
		}
		if (this->firstMove) {
			if (!board[X][Y + 1]) {
				moves.push_back(Coordinate(X, Y + 1));
			}
			if (!board[X][Y + 2] && !board[X][Y + 1]) {
				moves.push_back(Coordinate(X, Y + 2));
			}

			if (Y < _BOARD_WIDTH) {
				if (X < _BOARD_HEIGHT) {
					if (board[X + 1][Y + 1] && board[X + 1][Y + 1]->getColor() == Color::Black) {
						moves.push_back(Coordinate(X + 1, Y + 1));
					}
				}
				if (X > 0) {
					if (board[X - 1][Y + 1] && board[X - 1][Y + 1]->getColor() == Color::Black) {
						moves.push_back(Coordinate(X - 1, Y + 1));
					}
				}			
			}
		}
		else {
			if (Y < _BOARD_WIDTH) {
				if (!board[X][Y + 1]) {
					moves.push_back(Coordinate(X, Y + 1));
				}
			}
			if (Y < _BOARD_WIDTH) {
				if (X < _BOARD_HEIGHT) {
					if (board[X + 1][Y + 1] && board[X + 1][Y + 1]->getColor() == Color::Black) {
						moves.push_back(Coordinate(X + 1, Y + 1));
					}
				}
				if (X > 0) {
					if (board[X - 1][Y + 1] && board[X - 1][Y + 1]->getColor() == Color::Black) {
						moves.push_back(Coordinate(X - 1, Y + 1));
					}
				}
			}
			//en-passant			
			if (Y == 4) {
				if (X > 0) {
					if (board[X - 1][Y] && board[X - 1][Y]->getColor() == Color::Black && board[X - 1][Y]->getType() == PieceType::Pawn) {
						Pawn* enemyPawn = (Pawn*)board[X - 1][Y];

						if (enemyPawn && enemyPawn->getEnableEnPassantCaptured()) {
							moves.push_back(Coordinate(X - 1, Y + 1));
						}
					}
				}
				if (X < _BOARD_HEIGHT) {
					if (board[X + 1][Y] && board[X + 1][Y]->getColor() == Color::Black && board[X + 1][Y]->getType() == PieceType::Pawn) {
						Pawn* enemyPawn = (Pawn*)board[X + 1][Y];

						if (enemyPawn && enemyPawn->getEnableEnPassantCaptured()) {
							moves.push_back(Coordinate(X + 1, Y + 1));
						}
					}
				}
			}
		}
	}
	else if (this->getColor() == Color::Black) {
		if (Y == 0) {
			return result;
		}
		if (this->firstMove) {
			if (!board[X][Y - 1]) {
				moves.push_back(Coordinate(X, Y - 1));
			}
			if (!board[X][Y - 2] && !board[X][Y - 1]) {
				moves.push_back(Coordinate(X, Y - 2));
			}

			if (Y > 0) {
				if (X < _BOARD_HEIGHT) {
					if (board[X + 1][Y - 1] != nullptr && board[X + 1][Y - 1]->getColor() == Color::White) {
						moves.push_back(Coordinate(X + 1, Y - 1));
					}
				}
				if (X > 0) {
					if (board[X - 1][Y - 1] != nullptr && board[X - 1][Y - 1]->getColor() == Color::White) {
						moves.push_back(Coordinate(X - 1, Y - 1));
					}
				}
			}
		}
		else {
			if (Y > 0) {
				if (!board[X][Y - 1]) {
					moves.push_back(Coordinate(X, Y - 1));
				}
			}

			if (Y > 0) {
				if (X < _BOARD_HEIGHT) {
					if (board[X + 1][Y - 1] != nullptr && board[X + 1][Y - 1]->getColor() == Color::White) {
						moves.push_back(Coordinate(X + 1, Y - 1));
					}
				}
				if (X > 0) {
					if (board[X - 1][Y - 1] != nullptr && board[X - 1][Y - 1]->getColor() == Color::White) {
						moves.push_back(Coordinate(X - 1, Y - 1));
					}
				}
			}
			//en-passant
			if (Y == 3) {
				if (X > 0) {
					if (board[X - 1][Y] && board[X - 1][Y]->getColor() == Color::White && board[X - 1][Y]->getType() == PieceType::Pawn) {
						Pawn* enemyPawn = (Pawn*)board[X - 1][Y];

						if (enemyPawn && enemyPawn->getEnableEnPassantCaptured()) {
							moves.push_back(Coordinate(X - 1, Y - 1));
						}
					}
				}
				if (X < _BOARD_HEIGHT) {
					if (board[X + 1][Y] && board[X + 1][Y]->getColor() == Color::White && board[X + 1][Y]->getType() == PieceType::Pawn) {
						Pawn* enemyPawn = (Pawn*)board[X + 1][Y];

						if (enemyPawn && enemyPawn->getEnableEnPassantCaptured()) {
							moves.push_back(Coordinate(X + 1, Y - 1));
						}
					}
				}
			}
		}
	}

	this->getMovesWhenCheckmated(moves, board);

	for (auto& e : moves) {
		if (!board[e.getX()][e.getY()]) result[0].push_back(e);
		else result[1].push_back(e);
	}

	return result;
}
Piece* Pawn::clone() const{
	return new Pawn(*this);
}
void Pawn::write(std::fstream& os) {
	Piece::write(os);
	os.write((char*)&firstMove, sizeof(firstMove));
	os.write((char*)&enableEnPassantCaptured, sizeof(enableEnPassantCaptured));
}
void Pawn::read(std::fstream& is) {
	Piece::read(is);
	is.read((char*)&firstMove, sizeof(firstMove));
	is.read((char*)&enableEnPassantCaptured, sizeof(enableEnPassantCaptured));
}