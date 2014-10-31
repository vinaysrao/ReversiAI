#ifndef REVERSICOMPETITIONAGENT_H
#define REVERSICOMPETITIONAGENT_H

#include "coordinate.h"
#include "reversiboard.h"
#include "reversicommon.h"

#include <limits>

using namespace reversi;
using namespace std;

const double POS_INF = std::numeric_limits<double>::infinity();
const double NEG_INF = -POS_INF;

const int HEURISTIC[BOARD_SIZE][BOARD_SIZE] = {
    {80, -26, 24, -1, -5, 28, -18, 76},
    {-23, -39, -18, -9, -6, -8, -39, -1},
    {46, -16, 4, 1, -3, 6, -20, 52},
    {-13, -5, 2, -1, 4, 3, -12, -2},
    {-5, -6, 1, -2, -3, 0, -9, -5},
    {48, -13, 12, 5, 0, 5, -24, 41},
    {-27, -53, -11, -1, -11, -16, -58, -15},
    {87, -25, 27, -1, 5, 36, -3, 100}
};

class Node {
public:
    double value;
    double upperbound;
    double lowerbound;
    string state;
    Coordinate move;

    Node(double value, Coordinate move) : value(value), move(move) {
    }
};

class ReversiCompetitionAgent {
public:
    ReversiCompetitionAgent(vector< vector< char > >& currentState, char player, char opponent, double cpuTime);

    struct HeuristicCompare {
        bool operator()(Coordinate m1, Coordinate m2) {
            int x1 = m1.x, y1 = m1.y;
            int x2 = m2.x, y2 = m2.y;
            return HEURISTIC[x1][y1] > HEURISTIC[x2][y2];
        }
    } heuristicCompare;

    void play();

private:
    double cpuTime;
    ReversiBoard board;

    int m_player;
    int m_opponent;
    int cutoffDepth;

    bool isMaxPlayer(int player);

    int readMoves();
    void writeMoves(int moves);

    bool definitelyGreaterThan(float a, float b, float epsilon);

    Node iterativeDeepening(int depth, double alpha, double beta, Coordinate& move, int player);

    // alphabetasearch
    Node minMax(int depth, double alpha, double beta, Coordinate& move, int player);

    // calculate heuristic
    double evaluateScore(char player, Coordinate &action, vector<Coordinate> &playerMoves);

    // order valid moves
    void orderValidMoves(vector< Coordinate >& moves);

    void writeOutput(Coordinate& move);

    bool shouldStopSearch(int depth, vector<Coordinate> &moves);
};

#endif // REVERSICOMPETITIONAGENT_H
