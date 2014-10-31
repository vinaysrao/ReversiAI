#ifndef REVERSICOMMON_H
#define REVERSICOMMON_H

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <sstream>
#include <vector>

#define BOARD_SIZE 8
#define NO_OF_DIRECTIONS 8

using namespace std;

namespace reversi {
    static const char COLUMN_NAMES[BOARD_SIZE] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};

    static const int DIRECTIONS[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1}, {0, 1},
        {1, -1}, {1, 0}, {1, 1}
    };

    static const int INF_POS = std::numeric_limits<int>::max();
    static const int INF_NEG = std::numeric_limits<int>::min();

    static const int POS_WEIGHTS_HW[BOARD_SIZE][BOARD_SIZE] = {
        {99, -8, 8, 6, 6, 8, -8, 99},
        {-8, -24, -4, -3, -3, -4, -24, -8},
        {8, -4, 7, 4, 4, 7, -4, 8},
        {6, -3, 4, 0, 0, 4, -3, 6},
        {6, -3, 4, 0, 0, 4, -3, 6},
        {8, -4, 7, 4, 4, 7, -4, 8},
        {-8, -24, -4, -3, -3, -4, -24, -8},
        {99, -8, 8, 6, 6, 8, -8, 99}
    };

    class ReversiCommon {
    public:
        ReversiCommon(vector<vector<char> > &currentState, char player, char opponent, int task): m_player(player),
                m_opponent(opponent), task(task) {
            this->currentState = currentState;
            passes = 0;
        }
        void play();

        class Move {
        public:
            int x;
            int y;
            bool m_empty;
            Move(int x, int y, bool empty=false): x(x), y(y), m_empty(empty) {
            }
            bool empty() {
                return m_empty || passMove() || rootMove();
            }
            bool passMove() {
                return (x == -2 && y == -2);
            }
            bool rootMove() {
                return (x == -1 && y == -1);
            }
        };

        class MoveValue {
        public:
            int value;
            Move move;
            MoveValue(int value, Move &move): value(value), move(move) {
            }
            MoveValue(int value, int x, int y): value(value), move(x, y) {
            }
        };

        class MoveValueD {
        public:
            double value;
            Move move;
            MoveValueD(double value, Move &move): value(value), move(move) {
            }
            MoveValueD(double value, int x, int y): value(value), move(x, y) {
            }
        };

        class DiscsAndStableDiscs {
        public:
            map<char, int> numberOfDiscs;
            map<char, int> numberOfStableDiscs;
            DiscsAndStableDiscs(map<char, int> &numberOfDiscs, map<char, int> &numberOfStableDiscs):
                    numberOfDiscs(numberOfDiscs), numberOfStableDiscs(numberOfStableDiscs) {
            }
        };

        static int compareNumbers(int n1, int n2) {
            if (n1 < n2) {
                return -1;
            } else if (n1 > n2) {
                return 1;
            }
            return 0;
        }

        static int compareMoves(Move m1, Move m2) {
            if (m1.empty()) {
                return 1;
            }
            if (m2.empty()) {
                return -1;
            }
            int xCompared = compareNumbers(m1.x, m2.x);
            if (xCompared == 0) {
                return compareNumbers(m1.y, m2.y);
            }
            return xCompared;
        }

        static bool cornerOrEdge(int x, int y) {
            return (x == 0 || x == BOARD_SIZE -1 || y == 0 || y == BOARD_SIZE - 1);
        }

        /**
         * Checks that the given coordinates fall within the board
         */
        static bool checkMoveValid(int i, int j) {
            return (0 <= i && i < BOARD_SIZE && 0 <= j && j < BOARD_SIZE);
        }

        /**
         * Checks that the given coordinates fall within the board and if so, that
         * board(i, j) != symbol
         */
        static bool checkMoveValid(int i, int j, vector<vector<char> > &board, char symbol='*') {
            bool valid = checkMoveValid(i, j) && board[i][j] != symbol;
            return valid;
        }

        static int evaluateScoreHW(vector<vector<char> > &board) {
            int maxPlayerScore = 0;
            int minPlayerScore = 0;
            for (int i = 0; i < BOARD_SIZE; i++) {
                for (int j = 0; j < BOARD_SIZE; j++) {
                    if (board[i][j] == 'X') {
                        maxPlayerScore += POS_WEIGHTS_HW[i][j];
                    } else if (board[i][j] == 'O') {
                        minPlayerScore += POS_WEIGHTS_HW[i][j];
                    }
                }
            }
            return maxPlayerScore - minPlayerScore;
        }

        static string formatInfinity(int value) {
            if (value == INF_POS) {
                return "Infinity";
            } else if (value == INF_NEG) {
                return "-Infinity";
            } else {
                return toString(value);
            }
        }

        static string toString(int integer) {
            ostringstream ss;
            ss << integer;
            return ss.str();
        }

        static vector<string> formatLog(Move move, int depth, int value, int alpha, int beta) {
            vector<string> log;
            bool forceRoot = false;
            if (depth == 0) {
                forceRoot = true;
            }
            log.push_back(formatMove(move, forceRoot));
            log.push_back(toString(depth));
            log.push_back(formatInfinity(value));
            log.push_back(formatInfinity(alpha));
            log.push_back(formatInfinity(beta));
            return log;
        }

        static string formatMove(Move move, bool forceRoot=false) {
            if (move.rootMove() || forceRoot) {
                return string("root");
            } else if (move.passMove()) {
                return string("pass");
            }

            int x = move.x, y = move.y;
            char row = char('1') + x;
            char column = COLUMN_NAMES[y];
            char moveFormatted[2];
            sprintf(moveFormatted, "%c%c", column, row);
            return string(moveFormatted);
        }

        void logMove(Move move, int depth, int value, int alpha, int beta) {
            vector<string> log = formatLog(move, depth, value, alpha, beta);
            traverseLog.push_back(log);
        }

        static map<int, vector<int> > makeMove(Move move, vector<vector<char> > &board, char player) {
            map<int, vector<int> > flips;
            if (move.empty()) {
                return flips;
            }
            int x = move.x, y = move.y;
            board[x][y] = player;
            for (int d = 0; d < NO_OF_DIRECTIONS; d++) {
                int delta_i = DIRECTIONS[d][0], delta_j = DIRECTIONS[d][1];
                int i = x + delta_i, j = y + delta_j;
                bool foundPlayer = false;
                while (checkMoveValid(i, j, board) && board[i][j] != player) {
                    // First check if flips are possible in this direction
                    i += delta_i;
                    j += delta_j;
                    if (checkMoveValid(i, j) && board[i][j] == player) {
                        foundPlayer = true;
                        break;
                    }
                }
                if ((i != x + delta_i || j != y + delta_j) && foundPlayer) {
                    // Then make the flips if we found a piece of the same player
                    int a = x + delta_i, b = y + delta_j;
                    while (a != i || b != j) {
                        board[a][b] = player;
                        a += delta_i;
                        b += delta_j;
                    }
                    vector<int> lastFlipInDirection;
                    lastFlipInDirection.push_back(i);
                    lastFlipInDirection.push_back(j);
                    flips[d] = lastFlipInDirection;
                }
            }
            return flips;
        }

        static DiscsAndStableDiscs numberOfDiscsAndStableDiscs(vector<vector<char> > &board) {
            map<char, int> noOfDiscs;
            map<char, int> noOfStableDiscs;

            char player = 'X';
            char opponent = 'O';
            noOfDiscs[player] = 0;
            noOfDiscs[opponent] = 0;
            noOfStableDiscs[player] = 0;
            noOfStableDiscs[opponent] = 0;

            for (int i = 0; i < BOARD_SIZE; i++) {
                for (int j = 0; j < BOARD_SIZE; j++) {
                    if (board[i][j] != '*') {
                        noOfDiscs[board[i][j]] += 1;

                        if (cornerOrEdge(i, j)) {
                            noOfStableDiscs[board[i][j]] += 1;
                        } else {
                            int d = 0;
                            while (d < NO_OF_DIRECTIONS) {
                                // Check in that direction
                                int delta_i = DIRECTIONS[d][0], delta_j = DIRECTIONS[d][1];
                                int x = i + delta_i, y = j + delta_j;
                                while (checkMoveValid(x, y, board) && board[x][y] == board[i][j]) {
                                    x += delta_i;
                                    y += delta_j;
                                }
                                char opponentSymbol = board[i][j] == 'X' ? 'O' : 'X';
                                if (checkMoveValid(x, y) && board[x][y] == opponentSymbol) {
                                    break;
                                }
                                d++;
                            }
                            if (d == NO_OF_DIRECTIONS) {
                                // Then we've checked along all directions, and we reached a terminal state
                                noOfStableDiscs[board[i][j]] += 1;
                            }
                        }
                    }
                }
            }

            return DiscsAndStableDiscs(noOfDiscs, noOfStableDiscs);
        }

        static Move parseMove(string moveString) {
            moveString.erase(remove(moveString.begin(), moveString.end(), '\n'), moveString.end());
            moveString.erase(remove(moveString.begin(), moveString.end(), ' '), moveString.end());
            if (moveString.compare("root") == 0) {
                return Move(-1, -1, true);
            } else if (moveString.compare("pass") == 0) {
                return Move(-2, -2, true);
            } else {
                return Move((char) moveString[1] - '1', (char) moveString[0] - 'a');
            }
        }

        static void showBoard(vector<vector<char> > &board) {
            for (int i = 0; i < board.size(); i++) {
                vector<char> row = board[i];
                for (int j = 0; j < row.size(); j++) {
                    cout << board[i][j] << " ";
                }
                cout << endl;
            }
            cout << endl;
        }

        static void undoMove(Move move, vector<vector<char> > &board, char player, map<int, vector<int> > &flips) {
            int x = move.x, y = move.y;
            board[x][y] = '*';
            char opponent = player == 'X'? 'O': 'X';
            map<int, vector<int> >::iterator it;
            for (it = flips.begin(); it != flips.end(); it++) {
                int d = (*it).first;
                vector<int> lastFlip = (*it).second;
                int last_i = lastFlip[0], last_j = lastFlip[1];
                int delta_i = DIRECTIONS[d][0], delta_j = DIRECTIONS[d][1];
                int i = x + delta_i, j = y + delta_j;
                while (checkMoveValid(i, j, board) && (i != last_i || j != last_j)) {
                    board[i][j] = opponent;
                    i += delta_i;
                    j += delta_j;
                }
            }
        }

        static vector<Move> validMoves(vector<vector<char> > &board, char t_player) {
            vector<Move> validMovesList;
            for (int x = 0; x < BOARD_SIZE; x++) {
                for (int y = 0; y < BOARD_SIZE; y++) {
                    if (board[x][y] != '*') {
                        continue;
                    }
                    for (int d = 0; d < NO_OF_DIRECTIONS; d++) {
                        int delta_i = DIRECTIONS[d][0], delta_j = DIRECTIONS[d][1];
                        int i = x + delta_i, j = y + delta_j;
                        while (checkMoveValid(i, j, board) && board[i][j] != t_player) {
                            i += delta_i;
                            j += delta_j;
                        }
                        if ((i != x + delta_i || j != y + delta_j) && checkMoveValid(i, j) && board[i][j] == t_player) {
                            Move move(x, y);
                            validMovesList.push_back(move);
                            break;
                        }
                    }
                }
            }
            return validMovesList;
        }

        bool terminalTest(int depth, int cutoffDepth, vector<Move> &p_validMoves, char t_player) {
            char t_opponent = t_player == 'X'? 'O' : 'X';
            DiscsAndStableDiscs discs = numberOfDiscsAndStableDiscs(currentState);
            map<char, int> noOfDiscs = discs.numberOfDiscs;
            bool noPlayerDiscs = noOfDiscs[m_player] == 0 || noOfDiscs[m_opponent] == 0;
            return (depth >= cutoffDepth || (p_validMoves.size() == 0 && passes == 2) || noPlayerDiscs);
        }

    protected:
        vector<vector<char> > currentState;
        char m_player;
        bool prune;
        char m_opponent;
        int task;
        vector<vector<string> > traverseLog;
        int traverseLogLength;
        int passes;
    };
};

#endif // REVERSICOMMON_H
