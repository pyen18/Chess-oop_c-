#include "History.h"

History::History() {
    opponent = Opponent::HUMAN; //defualt
    initialState = nullptr;
    finalState = nullptr;
    capturedPiece = nullptr;
}

History::~History() {
    clear();
    initialState = nullptr;
    finalState = nullptr;
    capturedPiece = nullptr;
}

void History::write(std::string path) const {
    std::fstream os(path, std::fstream::out| std::fstream::binary);
    os.write((char*)&opponent, sizeof(opponent));

    int32_t size = this->history.size();
    // write int32_t size of vector to file
    os.write((char*) &size, sizeof(size));

    //write type and entire data of piece
    enum PieceType type;
    bool isNull;
    Piece* piece = nullptr;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < 3; j++) {
            piece = this->history[i][j];

            if (!piece) {
                isNull = true;
                os.write((char*)&isNull, sizeof(isNull));
                break;
            }
            else {
                isNull = false;
                os.write((char*)&isNull, sizeof(isNull));
            }

            type = piece->getType();
            os.write((char*)&type, sizeof(type));
            piece->write(os);
        }
    }
    os.close();
}

void History::read(std::string path) {
    this->clear();
    std::fstream is(path, std::fstream::in | std::fstream::binary);
    if (!is.is_open()) {
        std::string msg = "This item '" + path + "' doesn't exist\n";
        throw std::string(msg);
    }

    int32_t size;
    bool isNull;
    PieceType type;
    Piece* piece = nullptr;
    is.read((char*)&opponent, sizeof(opponent));
    is.read((char*)&size, sizeof(size));
    history.resize(size);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < 3; j++) {
            is.read((char*)&isNull, sizeof(isNull));
            if (isNull) {
                history[i].push_back(nullptr);
                break;
            }

            is.read((char*)&type, sizeof(type));
            switch (type) {
                case PieceType::King: {
                    King* king = new King();
                    king->read(is);
                    piece = king;
                    break;
                }
                case PieceType::Queen: {
                    Queen* queen = new Queen();
                    queen->read(is);
                    piece = queen;
                    break;
                }
                case PieceType::Bishop: {
                    Bishop* bishop = new Bishop();
                    bishop->read(is);
                    piece = bishop;
                    break;
                }
                case PieceType::Knight: {
                    Knight* knight = new Knight();
                    knight->read(is);
                    piece = knight;
                    break;
                }
                case PieceType::Rook: {
                    Rook* rook = new Rook();
                    rook->read(is);
                    piece = rook;
                    break;
                }
                case PieceType::Pawn: {
                    Pawn* pawn = new Pawn();
                    pawn->read(is);
                    piece = pawn;
                    break;
                }
            }
          
            history[i].push_back(piece);
        }
    }
    is.close();
} 

void History::setOpponent(Opponent opponent){
    this->opponent = opponent;
}

Opponent History::getOpponent() {
    return this->opponent;
}

void History::setInitialState(const Piece* initialState) {
    if (!initialState) {
        this->initialState = nullptr;
        return;
    }
 
    this->initialState = initialState->clone();
}

void History::setFinalState(const Piece* finalState) {
    if (!finalState) {
        this->finalState = nullptr;
        return;
    }

    this->finalState = finalState->clone();
}

void History::setCapturedPiece(const Piece* capturedPiece) {
    if (!capturedPiece) {
        this->capturedPiece = nullptr;
        return;
    }

    this->capturedPiece = capturedPiece->clone();
}

void History::updateData(int turn) {
    for (int i = turn; i < history.size(); i++) {
        for (int j = 0; j < 3; j++) {
            if (history[i][j]) {
                delete history[i][j];
                history[i][j] = nullptr;
            }
        }
    }
    if (turn < history.size()) history.erase(history.begin() + turn, history.end());
    history.push_back(std::vector<Piece*>{ initialState, finalState, capturedPiece });
}

std::vector<Piece*> History::getData(int turn) {
    return history[turn];
}

int History::getLengthData() {
    return history.size();
}

void History::clear() {
    for (auto& v : history) {
        for (auto& u : v) {
            if (u) {
                delete u;
                u = nullptr;
            }
        }
    }
    history.clear();
}