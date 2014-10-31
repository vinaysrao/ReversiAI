#include "reversihwagent.h"

#include <iostream>
#include <fstream>

using namespace std;

ReversiHWAgent::ReversiHWAgent(int cutoffDepth, vector<vector<char> > &currentState, char player, char opponent, int task):
        cutoffDepth(cutoffDepth), ReversiCommon(currentState, player, opponent, task) {
    prune = false;
    if (task == 1) {
        // Greedy
        cout << "Greedy" << endl;
        cutoffDepth = 1;
        traverseLogLength = 0;
    } else if (task == 2) {
        // MiniMax
        cout << "MiniMax" << endl;
        traverseLogLength = 3;
    } else if (task == 3) {
        // AlphaBeta
        cout << "AlphaBeta" << endl;
        traverseLogLength = 5;
        prune = true;
    }
}

reversi::ReversiCommon::MoveValue ReversiHWAgent::minMax(int depth, int alpha, int beta, reversi::ReversiCommon::Move move,
                                                        bool maxPlayer) {
    char maxSymbol = maxPlayer? player : opponent;
    // First get the valid moves
    vector<Move> c_validMoves = validMoves(currentState, maxSymbol);
    int value = maxPlayer? reversi::INF_NEG : reversi::INF_POS;

    // If the game ends (both players don't have moves or cutoffDepth is reached)
    if (terminalTest(depth, cutoffDepth, c_validMoves, maxSymbol)) {
        // Then we're at a leaf node with no more moves
        value = evaluateScoreHW(currentState);
        logMove(move, depth, value, alpha, beta);
        return MoveValue(value, move);
    }

    logMove(move, depth, value, alpha, beta);
    // Else, the current player might have no moves, so we can pass over
    if(c_validMoves.size() == 0) {
        // Then the game hasn't ended, and we can still perform checks for the other player
        Move bestMove(-2, -2, true);
        Move passMove(-2, -2, true);
        passes += 1;
        MoveValue moveValue = minMax(depth + 1, alpha, beta, bestMove, !maxPlayer);
        if ((maxPlayer && moveValue.value > value) || (!maxPlayer && moveValue.value < value)) {
            bestMove = moveValue.move;
            value = moveValue.value;
        }
        if(prune) {
            if ((maxPlayer && value >= beta) || (!maxPlayer && value <= alpha)) {
                logMove(move, depth, value, alpha, beta);
                return MoveValue(value, passMove);
            }
            if(maxPlayer) {
                alpha = max(alpha, value);
            } else {
                beta = min(beta, value);
            }
        }
        logMove(move, depth, value, alpha, beta);
        return MoveValue(value, passMove);
    }

    passes = 0;

    Move bestMove(-2, -2, true);
    if (depth == 0) {
        bestMove = Move(-1, -1, true);
    }

    for (int m = 0; m < c_validMoves.size(); m++) {
        Move action = c_validMoves[m];

        map<int, vector<int> > flips = makeMove(action, currentState, maxSymbol);
        MoveValue childMoveValue = minMax(depth + 1, alpha, beta, action, !maxPlayer);
        undoMove(action, currentState, maxSymbol, flips);

        if ((maxPlayer && childMoveValue.value > value) || (!maxPlayer && childMoveValue.value < value)) {
            bestMove = action;
            value = childMoveValue.value;
        }
        if(prune) {
            if ((maxPlayer && value >= beta) || (!maxPlayer && value <= alpha)) {
                logMove(move, depth, value, alpha, beta);
                return MoveValue(value, bestMove);
            }
            if(maxPlayer) {
                alpha = max(alpha, value);
            } else {
                beta = min(beta, value);
            }
        }
        logMove(move, depth, value, alpha, beta);
    }
    return MoveValue(value, bestMove);
}


void ReversiHWAgent::play() {
    vector<string> headers;
    headers.push_back("Node");
    headers.push_back("Depth");
    headers.push_back("Value");
    headers.push_back("Alpha");
    headers.push_back("Beta");
    traverseLog.push_back(headers);

    int alpha = reversi::INF_NEG, beta = reversi::INF_POS;
    Move move(-1, -1, true);
    MoveValue moveValue = minMax(0, alpha, beta, move, true);
    cout << formatMove(moveValue.move) << endl;
    if (!moveValue.move.empty()) {
        Move move = moveValue.move;
        makeMove(move, currentState, player);
    }
    writeOutput();
}

void ReversiHWAgent::writeOutput() {
    ofstream outputFile("output.txt");
    if(!outputFile.is_open()) {
        cout << "Failed to write output to: output.txt" << endl;
        return;
    }
    // Write the board state
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            outputFile << currentState[i][j];
        }
        outputFile << endl;
    }
    if(task != 1) {
        for (int i = 0; i < traverseLog.size(); i++) {
            vector<string> parameters = traverseLog[i];
            string outputString = "";
            for (int j = 0; j < traverseLogLength; j++) {
                outputString += parameters[j];
                if (j != traverseLogLength - 1) {
                    outputString += ",";
                }
            }
            outputFile << outputString << endl;
        }
    }
    outputFile << endl;
    outputFile.close();
}
