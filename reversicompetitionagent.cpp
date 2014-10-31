#include "reversicompetitionagent.h"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <limits>

using namespace reversi;
using namespace std;

ReversiCompetitionAgent::ReversiCompetitionAgent(vector< vector< char > >& currentState, char player, char opponent, double cpuTime):
                                                 cpuTime(cpuTime), board(currentState) {
    if (player == 'X') {
        m_player = 0;
        m_opponent = 1;
        cutoffDepth = 4;
    } else {
        cutoffDepth = 5;
        m_player = 1;
        m_opponent = 0;
    }
}

double ReversiCompetitionAgent::evaluateScore(char player, Coordinate& action, vector< Coordinate >& playerMoves) {
    int opponent = 1 - player;

    // Make the move and get the number of moves the opponent can make
    vector<Coordinate> opponentMoves = board.legalMoves(opponent);

    // Mobility ratio or number of moves
    double noOfPlayerMoves = max(1.0, (double) playerMoves.size());
    double noOfOpponentMoves = max(1.0, (double) opponentMoves.size());
    double mobilityRatio = noOfPlayerMoves / noOfOpponentMoves;

    // Heuristic
    double heuristic = 1.0;
    if (!action.empty()) {
        heuristic = (double) HEURISTIC[action.x][action.y];
    }

    // Disc ratio
    double noOfPlayerDiscs = max(1.0, (double) board.numberOfPieces(player));
    double noOfOpponentDiscs = max(1.0, (double) board.numberOfPieces(opponent));
    double discRatio = noOfPlayerDiscs / noOfOpponentDiscs;

    // Stable disc ratio: count the number of unflippable discs and get the ratio
    double noOfPlayerStableDiscs = max(1.0, (double) board.numberOfStablePieces(player));
    double noOfOpponentStableDiscs = max(1.0, (double) board.numberOfStablePieces(opponent));
    double stableDiscRatio = noOfPlayerStableDiscs / noOfOpponentStableDiscs;

    double value = (heuristic * discRatio * stableDiscRatio * mobilityRatio);

    return value;
}

void ReversiCompetitionAgent::orderValidMoves(vector< Coordinate >& moves) {
    // Order moves according to their current heuristic value
    std::sort(moves.begin(), moves.end(), heuristicCompare);
}

bool ReversiCompetitionAgent::isMaxPlayer(int player) {
    return player == m_player;
}

Node ReversiCompetitionAgent::iterativeDeepening(int depth, double alpha, double beta, Coordinate &move, int player) {
    chrono::time_point<chrono::system_clock> playerStart, playerEnd;
    int moves = readMoves();
    double timeRemaining = cpuTime / ((double) (max(1, 32 - moves)));
    playerStart = chrono::system_clock::now();
    Node node(NEG_INF, Coordinate(-2, -2));
    for (int d = 2; d < 8; d++) {
        cutoffDepth = d;
        Node newNode = minMax(d, alpha, beta, move, player);
        if (newNode.value > node.value) {
            node = newNode;
        }
        playerEnd = chrono::system_clock::now();
        chrono::duration<double> playerDuration = playerEnd - playerStart;
        double playerSeconds = playerDuration.count();
        cout << timeRemaining << " " << playerSeconds << " " << (!definitelyGreaterThan(timeRemaining, playerSeconds, 0.5)) << endl;
        if (!definitelyGreaterThan(timeRemaining, playerSeconds, 0.5)) {
            writeMoves(moves + 1);
            cout << endl;
            return node;
        }
    }
    cout << endl;
    writeMoves(moves + 1);
    return node;
}

bool ReversiCompetitionAgent::definitelyGreaterThan(float a, float b, float epsilon)
{
    return (a - b) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

int ReversiCompetitionAgent::readMoves() {
    char moveFormatted[18];
    sprintf(moveFormatted, "numberofmoves%d.txt", m_player);
    ifstream inputFile(string(moveFormatted).c_str());
    if (!inputFile.is_open()) {
        writeMoves(1);
        return 0;
    }
    int moves;
    inputFile >> moves;
    inputFile.close();
    return moves;
}

void ReversiCompetitionAgent::writeMoves(int moves) {
    char moveFormatted[18];
    sprintf(moveFormatted, "numberofmoves%d.txt", m_player);
    ofstream outputFile(string(moveFormatted).c_str());
    outputFile << (moves);
    outputFile.close();
}


void ReversiCompetitionAgent::play() {
    double alpha = NEG_INF, beta = POS_INF;
    Coordinate move(-2, -2);
    Node node = minMax(0, alpha, beta, move, m_player);
    writeOutput(node.move);
}

Node ReversiCompetitionAgent::minMax(int depth, double alpha, double beta, Coordinate& move, int player) {
    // First get the valid moves
    vector<Coordinate> playerMoves = board.legalMoves(player);
    double value;
//     cout << m_player << " " << player << " " << move.toString() << " : ";
//     for (Coordinate c: playerMoves) {
//         cout << c.toString() << " ";
//     }
//     cout << endl;

    if (shouldStopSearch(depth, playerMoves)) {
        value = evaluateScore(player, move, playerMoves);
        return Node(value, move);
    }

    bool maxPlayer = isMaxPlayer(player);
    value = maxPlayer ? NEG_INF : POS_INF;
    Coordinate bestMove(-2, -2);

    for (Coordinate action: playerMoves) {
        ReversiBoard copyBoard = board.clone();

        board.makeMove(player, action);
        Node childNode = minMax(depth + 1, alpha, beta, action, 1 - player);
        board = copyBoard;

        if ((maxPlayer && childNode.value > value) || (!maxPlayer && childNode.value < value)) {
            bestMove = action;
            value = childNode.value;
        }
        if ((maxPlayer && value >= beta) || (!maxPlayer && value <= alpha)) {
            return Node(value, bestMove);
        }
        if(maxPlayer) {
            alpha = max(alpha, value);
        } else {
            beta = min(beta, value);
        }
    }
    return Node(value, bestMove);
}

bool ReversiCompetitionAgent::shouldStopSearch(int depth, vector< Coordinate >& moves) {
    // See if we should stop searching considering the depth and the number of valid moves we have
    if (moves.size() == 0 || depth >= cutoffDepth) {
        return true;
    }
    return false;
}

void ReversiCompetitionAgent::writeOutput(Coordinate &move) {
    ofstream outputFile("output.txt");
    if(!outputFile.is_open()) {
        cout << "Failed to write output to: output.txt" << endl;
        return;
    }
    outputFile << move.toString();
    outputFile.close();
}

