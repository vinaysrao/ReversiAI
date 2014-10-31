#ifndef REVERSIBOARD_H
#define REVERSIBOARD_H

#include "coordinate.h"

#include <vector>

#define BOARD_SIZE 8

using namespace std;

typedef unsigned long long int ullint;
typedef ullint (*shiftFunction) (ullint);

static const signed long long int LEFT_MASK = -9187201950435737472L;
static const signed long long int RIGHT_MASK = 72340172838076673L;

ullint shiftDown(ullint position);
ullint shiftDownLeft(ullint position);
ullint shiftDownRight(ullint position);
ullint shiftLeft(ullint position);
ullint shiftRight(ullint position);
ullint shiftUp(ullint position);
ullint shiftUpLeft(ullint position);
ullint shiftUpRight(ullint position);

class ReversiBoard
{
public:
    static const short int BLACK = 0;
    static const short int WHITE = 1;
    static const int DIRECTIONS = 8;

    static const ullint INITIAL_POSITION_BLACK = 68853694464L;
    static const ullint INITIAL_POSITION_WHITE = 34628173824L;

    static const ullint CORNERS = 9295429630892703873L;

    ullint pieces[2];

    ReversiBoard();
    ReversiBoard(vector< vector< char > >& board);
    ReversiBoard(ullint blackPieces, ullint whitePieces);

    vector<ullint> charBoardToLong(vector< vector< char > >& board);

    ReversiBoard clone();

    ullint blackPieces();

    ullint whitePieces();

    ullint blankBoard();

    ullint highestOrderBit(ullint num);

    Coordinate longToCoordinate(ullint position);

    ullint coordinateToLong(Coordinate c);

    vector<Coordinate> longToCoordinateList(ullint position);

    bool isCorner(ullint position);

    bool isMoveLegal(int color, Coordinate coordinate);

    vector<Coordinate> legalMoves(int player);

    bool makeMove(int color, Coordinate coordinate);

    vector<Coordinate> moveEndPoints(int color, Coordinate coordinate);

    /**
     * Takes the player and a starting position with a shift function.
     * Returns the end point (or last point that we can flip in the given direction)
     * If no flips can be made, returns -1
     */
    Coordinate moveEndPointInDirection(int color, ullint startPosition, shiftFunction func);

    vector<Coordinate> movesInDirection(int player, shiftFunction func);

    int numberOfPieces(int player);

    int numberOfStablePieces(int player);

    void setPieceAtPosition(int color, ullint position);

    void printBoard();

    bool validEndPoint(int color, ullint position);

    string bitToString(ullint pieces);
};


#endif // REVERSIBOARD_H
