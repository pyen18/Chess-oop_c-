#include "Computer.h"

Computer::Computer() {

}
// defaul, com 1st, 0;
std::pair<int, Coordinate> Computer::playWithEasyMode()
{
    srand(time(NULL));
    std::vector<int> enableMovingPieces;
    std::vector<std::vector<Coordinate>> moves;
    for (int i = 0; i < 16; i++) {
        Piece* piece = Board::piecesList[i];
        if (piece->getDead()) continue;
        piece->setChosen(true);
        std::vector<std::vector<Coordinate>> temp = piece->getPossibleMoves(Board::piecesOnBoard);
        piece->setChosen(false);
        std::vector<Coordinate> tempMoves;
        tempMoves.reserve(temp[0].size() + temp[1].size());
        copy(temp[0].begin(), temp[0].end(), std::back_inserter(tempMoves));
        copy(temp[1].begin(), temp[1].end(), std::back_inserter(tempMoves));

        if (tempMoves.size() >= 1) {
            enableMovingPieces.push_back(i);
            moves.push_back(tempMoves);
            tempMoves.clear();
        }
    }
    
    if (enableMovingPieces.size() == 0) throw std::string("Don't have any moves\n!!!");

    // random piece;
    int pieceIdx = rand() % enableMovingPieces.size();
    // random move for piece;
    int moveIdx = rand() % moves[pieceIdx].size();

  
    return std::pair<int, Coordinate>(enableMovingPieces[pieceIdx], moves[pieceIdx][moveIdx]);
}

std::pair<int, Coordinate> Computer::evadeEaten() {

    std::vector<Coordinate> moves;
    for (int i = 16; i < 32; i++) {
        Piece* piece = Board::piecesList[i];
        if (piece->getDead()) continue;
        std::vector<Coordinate> temp = piece->getPossibleMoves(Board::piecesOnBoard)[1];
        if (temp.size() == 0) continue;
        moves.insert(moves.end(), temp.begin(), temp.end());
    }

    for (int i = 0;i < 8;i++) {
        Piece* piece = Board::piecesList[i];
        for (auto u1 : moves) {
            if (u1 == piece->getPosition()) {
                return getNonEatenMove(i);
            }
        }
    }

    return std::pair<int, Coordinate>(-1, Coordinate(-1, -1));
}

std::pair<int,Coordinate> Computer::getNonEatenMove(int idx) {
    std::vector<Coordinate> movesOfPiece;
    Piece* chosenPiece = Board::piecesList[idx];
    chosenPiece->setChosen(true);
    movesOfPiece = chosenPiece->getPossibleMoves(Board::piecesOnBoard)[0];
    chosenPiece->setChosen(false);

    for (auto u : movesOfPiece) {
        std::vector<Coordinate> moves;
        Piece* buffer = Board::piecesOnBoard[u.getX()][u.getY()] ;
        Board::piecesOnBoard[chosenPiece->getPosition().getX()][chosenPiece->getPosition().getY()] = nullptr;
        Board::piecesOnBoard[u.getX()][u.getY()] = chosenPiece;

        for (int i = 16; i < 32; i++) {
            Piece* piece = Board::piecesList[i];
            if (piece->getDead()) continue;
            piece->setChosen(true);
            std::vector<Coordinate> temp = piece->getPossibleMoves(Board::piecesOnBoard)[1];
            piece->setChosen(false);
            if (temp.size() == 0) continue;
            moves.insert(moves.end(), temp.begin(), temp.end());
        }

        Board::piecesOnBoard[u.getX()][u.getY()] = buffer;
        Board::piecesOnBoard[chosenPiece->getPosition().getX()][chosenPiece->getPosition().getY()] = chosenPiece;

        int flag = true;
        for (int i = 0; i < moves.size(); i++) {
            if (u == moves[i]) flag = false;
        }

        if(flag) return  std::pair<int, Coordinate>(idx, u);
    }

    return std::pair<int, Coordinate>(-1, Coordinate(-1,-1));  
}

std::pair<int, Coordinate> Computer::playWithHardMode() {
    srand(time(NULL));
    std::vector<int> enableMovingPieces;
    std::vector<std::vector<std::vector<Coordinate>>> moves;

    for (int i = 0; i < 16; i++) {
        Piece* piece = Board::piecesList[i];
        if (piece->getDead()) continue;
        piece->setChosen(true);
        std::vector<std::vector<Coordinate>> temp = piece->getPossibleMoves(Board::piecesOnBoard);
        piece->setChosen(false);

        if (temp[0].size() + temp[1].size() >= 1) {
            enableMovingPieces.push_back(i);
            moves.push_back(temp);
        }
    }

    if (enableMovingPieces.size() == 0) throw std::string("Don't have any moves\n!!!");

    std::vector<PieceType> type{ PieceType::King,PieceType::Queen,PieceType::Rook,PieceType::Bishop,PieceType::Knight, PieceType::Pawn};
    
    //eating move
    for (auto e : type) {
        for (int i = 0; i < moves.size(); i++) {
            for (int j = 0; j < moves[i][1].size(); j++) {
                Piece* piece = Board::getPieceAt(moves[i][1][j]);
                if (piece->getType() == e && (e != PieceType::Pawn || Board::piecesList[enableMovingPieces[i]]->getType() == PieceType::Pawn)) {
                    return std::pair<int, Coordinate>(enableMovingPieces[i], moves[i][1][j]);
                }
            }
        }
    }

    std::pair<int, Coordinate> res;
    
    // evading move
    res = evadeEaten();
    if (res.first >= 0) return res;
  
    //  non-eaten move
    for (int i = 0; i < 7; i ++) {
        int pieceIdx = rand() % enableMovingPieces.size();
        res = getNonEatenMove(enableMovingPieces[pieceIdx]);
        if (res.first >= 0) return res;
    }
    
    // random piece;
    int pieceIdx = rand() % enableMovingPieces.size();
    // random move for piece;
    std::vector<std::vector<Coordinate>> combiningVector;
    combiningVector.resize(moves.size());
    for (int i = 0; i < moves.size(); i++) {
        combiningVector.reserve(moves[i][0].size() + moves[i][1].size());
        copy(moves[i][0].begin(), moves[i][0].end(), std::back_inserter(combiningVector[i]));
        copy(moves[i][1].begin(), moves[i][1].end(), std::back_inserter(combiningVector[i]));
    }
    int moveIdx = rand() % combiningVector[pieceIdx].size();
    return std::pair<int, Coordinate>(enableMovingPieces[pieceIdx], combiningVector[pieceIdx][moveIdx]);
}