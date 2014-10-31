#include "reversiboard.h"

#include <bitset>
#include <math.h>
#include <set>
#include <iostream>

using namespace std;

static shiftFunction shiftFunctions[8] = {
    shiftDown, shiftDownLeft, shiftDownRight, shiftLeft, shiftRight,
    shiftUp, shiftUpLeft, shiftUpRight
};

ReversiBoard::ReversiBoard() {
    ReversiBoard(INITIAL_POSITION_BLACK, INITIAL_POSITION_WHITE);
}

ReversiBoard::ReversiBoard(vector<vector<char> > &board) {
    vector<ullint> longPieces = charBoardToLong(board);
    pieces[BLACK] = longPieces[BLACK];
    pieces[WHITE] = longPieces[WHITE];
}

ReversiBoard::ReversiBoard(ullint blackPieces, ullint whitePieces) {
    pieces[BLACK] = blackPieces;
    pieces[WHITE] = whitePieces;
}

ReversiBoard ReversiBoard::clone() {
    return ReversiBoard(pieces[BLACK], pieces[WHITE]);
}

vector< ullint > ReversiBoard::charBoardToLong(vector<vector<char> > &board) {
    ullint longReprX = 0;
    ullint longReprO = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j] == 'X') {
                longReprX += (ullint) pow(2, 63 - (i * 8 + j));
            } else if (board[i][j] == 'O') {
                longReprO += (ullint) pow(2, 63 - (i * 8 + j));
            }
        }
    }
    vector<ullint> representations;
    representations.push_back(longReprX);
    representations.push_back(longReprO);

    return representations;
}

ullint ReversiBoard::blackPieces() {
    return pieces[BLACK];
}

ullint ReversiBoard::whitePieces() {
    return pieces[WHITE];
}

ullint ReversiBoard::blankBoard() {
    return ~blackPieces() & ~whitePieces();
}

ullint ReversiBoard::coordinateToLong(Coordinate c) {
    ullint l = (ullint) (pow(2, 63 - (c.x * 8 + c.y)));
    return l;
}

bool ReversiBoard::isMoveLegal(int color, Coordinate coordinate) {
    vector<Coordinate> moves = legalMoves(color);
    for (auto move: moves) {
        if (move == coordinate) {
            return true;
        }
    }
    return false;
}

bool ReversiBoard::isCorner(ullint position) {
    return (bool) position & CORNERS;
}

Coordinate ReversiBoard::longToCoordinate(ullint position) {
    Coordinate theCoordinate;
    if (position > 0) {
        ullint l = 1L;
        int counter = 0;
        while (theCoordinate.empty() && counter < 64){
            if (l == position) {
                return Coordinate(7 - counter / 8, 7 - counter % 8);
            }
            l = l << 1;
            counter++;
        }
    }
    return theCoordinate;
}

ullint ReversiBoard::highestOrderBit(ullint num) {
    if (!num)
        return 0;
    ullint ret = 1;
    while (num >>= 1)
        ret <<= 1;
    return ret;
}

vector< Coordinate > ReversiBoard::longToCoordinateList(ullint position) {
    ullint workingPosition = position;
    vector<Coordinate> coordinates;
    ullint highestOneBit = highestOrderBit(workingPosition);
    while (highestOneBit != 0) {
        coordinates.push_back(longToCoordinate(highestOneBit));
        workingPosition ^= highestOneBit;
        highestOneBit = highestOrderBit(workingPosition);
    }
    return coordinates;
}

bool ReversiBoard::makeMove(int color, Coordinate coordinate) {
    vector<Coordinate> piecesToTurn;
    piecesToTurn.push_back(coordinate);
    vector<Coordinate> endPoints = moveEndPoints(color, coordinate);
    piecesToTurn.insert(piecesToTurn.end(), endPoints.begin(), endPoints.end());
    for (auto c: endPoints) {
        vector<Coordinate> betweenEndsAndC = coordinate.between(c);
        piecesToTurn.insert(piecesToTurn.end(), betweenEndsAndC.begin(), betweenEndsAndC.end());
    }
    for (auto c: piecesToTurn) {
        setPieceAtPosition(color, coordinateToLong(c));
    }
//     printBoard();
}

vector< Coordinate > ReversiBoard::moveEndPoints(int color, Coordinate coordinate) {
    vector<Coordinate> endPoints;
    ullint startPoint = coordinateToLong(coordinate);

    for (int i = 0; i < 8; i++) {
        shiftFunction func = shiftFunctions[i];
        Coordinate endPoint = moveEndPointInDirection(color, startPoint, func);
        if (!endPoint.empty()) {
            endPoints.push_back(endPoint);
        }
    }
    return endPoints;
}

Coordinate ReversiBoard::moveEndPointInDirection(int color, ullint startPosition, shiftFunction func) {
    ullint potentialEndPoint = func(startPosition);
    while(potentialEndPoint != 0) {
        if(validEndPoint(color, potentialEndPoint)) {
            return longToCoordinate(potentialEndPoint);
        }
        potentialEndPoint = func(potentialEndPoint);
    }
    return longToCoordinate(-1);
}

vector< Coordinate > ReversiBoard::legalMoves(int player) {
    vector<Coordinate> moves;
    for (shiftFunction func: shiftFunctions) {
        vector<Coordinate> directionMoves = movesInDirection(player, func);
        moves.insert(moves.end(), directionMoves.begin(), directionMoves.end());
    }

    set<Coordinate> setOfMoves(moves.begin(), moves.end());
    moves.assign(setOfMoves.begin(), setOfMoves.end());
    return moves;
}

vector< Coordinate > ReversiBoard::movesInDirection(int player, shiftFunction func) {
    vector<Coordinate> moves;
    int otherPlayer = 1 - player;
    ullint potentialMoves = func(pieces[player]) & pieces[otherPlayer];
    ullint emptyBoard = blankBoard();
    while (potentialMoves != 0){
        ullint legalMoves = func(potentialMoves) & emptyBoard;
        vector<Coordinate> legalMovesList = longToCoordinateList(legalMoves);
        moves.insert(moves.end(), legalMovesList.begin(), legalMovesList.end());
        potentialMoves = func(potentialMoves) & pieces[otherPlayer];
    }
    return moves;
}

int ReversiBoard::numberOfPieces(int player) {
    ullint playerPieces = pieces[player];

    int noOfPieces = 0;
    while (playerPieces) {
        noOfPieces++;
        playerPieces ^= highestOrderBit(playerPieces);
    }

    return noOfPieces;
}

int ReversiBoard::numberOfStablePieces(int player) {
    int opponent = 1 - player;
    ullint playerPieces = pieces[player];
    ullint opponentPieces = pieces[opponent];
    int stablePieces = 0;

    while (playerPieces) {
        ullint highestBit = highestOrderBit(playerPieces);
        playerPieces ^= highestBit;
        if (isCorner(highestBit)) {
            stablePieces++;
        } else {
            // Go through all the directions and check if it can be flipped at any point of time
            bool isStable = true;
            for (shiftFunction func: shiftFunctions) {
                ullint position = func(highestBit);
                while (position && (position & pieces[player])) {
                    position = func(position);
                }
                if (position == func(highestBit)) {
                    isStable = false;
                    break;
                }
            }
            if (isStable) {
                stablePieces++;
            }
        }
    }

}

void ReversiBoard::printBoard() {
    string black = bitToString(pieces[BLACK]);
    string white = bitToString(pieces[WHITE]);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            char b = black.at(i * 8 + j);
            char w = white.at(i * 8 + j);
            if (b == '1') {
                cout << "X ";
            } else if (w == '1') {
                cout << "O ";
            } else {
                cout << "* ";
            }
        }
        cout << endl;
    }
    cout << endl;
}

string ReversiBoard::bitToString(ullint pieces) {
    return bitset<64>(pieces).to_string();
}

void ReversiBoard::setPieceAtPosition(int color, ullint position) {
    pieces[1 - color] &= ~position; 
    pieces[color] = pieces[color] | position; 
}

bool ReversiBoard::validEndPoint(int color, ullint position) {
    return (position & pieces[color]);
}

ullint shiftDown(ullint position) {
    return (ullint) (position >> 8);
}

ullint shiftDownLeft(ullint position) {
    ullint dlShift = (ullint) (position >> 7);
    return dlShift & ~RIGHT_MASK;
}

ullint shiftDownRight(ullint position) {
    ullint drShift = (ullint) (position >> 9);
    return drShift & ~LEFT_MASK;
}

ullint shiftLeft(ullint position) {
    ullint lShift = position << 1;
    return lShift & ~RIGHT_MASK;
}

ullint shiftRight(ullint position) {
    ullint rShift = (ullint) (position >> 1);
    return rShift & ~LEFT_MASK;
}

ullint shiftUp(ullint position) {
    return position << 8;
}

ullint shiftUpLeft(ullint position) {
    ullint ulShift = position << 9;
    return ulShift & ~RIGHT_MASK;
}

ullint shiftUpRight(ullint position) {
    ullint urShift = position << 7;
    return urShift & ~LEFT_MASK;
}
