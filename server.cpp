#include "server.h"
#include "reversicompetitionagent.h"

#include <chrono>
#include <iostream>
#include <fstream>

using namespace std;
using namespace reversi;

Server::Server() {
    // Competition task for the agent
    task = 4;

    initBoard();
}

void Server::initBoard() {
    boardState.clear();
    for (int i = 0; i < BOARD_SIZE; i++) {
        vector<char> row;
        for (int j = 0; j < BOARD_SIZE; j++) {
            row.push_back(INIT_BOARD_STATE[i][j]);
        }
        boardState.push_back(row);
    }
}


vector< vector< char > > Server::cloneBoard() {
    vector<vector<char> > clonedBoard;
    for (int i = 0; i < BOARD_SIZE; i++) {
        vector<char> row;
        for (int j = 0; j < BOARD_SIZE; j++) {
            row.push_back(boardState[i][j]);
        }
        clonedBoard.push_back(row);
    }
    return clonedBoard;
}

void Server::generateInput(int task, double cpuTime, char player) {
    ofstream outputFile("input.txt");

    outputFile << task << endl;
    outputFile << player << endl;
    outputFile << task << cpuTime << endl;

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            outputFile << boardState[i][j] << ' ';
        }
        outputFile << endl;
    }

    outputFile << endl;
    outputFile.close();
}

void Server::makePlay() {
    char player1Symbol = 'X';
    char player2Symbol = 'O';
    double player1Time = 200.0;
    double player2Time = 200.0;

    int noOfPlayer1Moves = 0;
    int noOfPlayer2Moves = 0;

    bool bothPlayersPassed = false;
    int counter = 0;

    while(!bothPlayersPassed) {
        // Pass parameters to player 1 and get move
        chrono::time_point<chrono::system_clock> player1Start, player1End;
        vector<vector<char> > player1Board = cloneBoard();
        ReversiCompetitionAgent player1Agent(player1Board, player1Symbol, player2Symbol, player1Time);

        player1Start = chrono::system_clock::now();
        player1Agent.play();
        player1End = chrono::system_clock::now();
        chrono::duration<double> player1Duration = player1End - player1Start;
        double player1Seconds = player1Duration.count();
        player1Time -= player1Seconds;


        ReversiCommon::Move player1Move = readMove();
        PlayerMoveTime player1MoveTime(player1Symbol, player1Move, player1Seconds);
        gameLog.push_back(player1MoveTime);

        if (!player1Move.passMove()) {
            noOfPlayer1Moves += 1;
        }
        ReversiCommon::makeMove(player1Move, boardState, player1Symbol);

        // Pass parameters to player 2 and get move
        chrono::time_point<chrono::system_clock> player2Start, player2End;
        vector<vector<char> > player2Board = cloneBoard();
        ReversiCompetitionAgent player2Agent(player2Board, player2Symbol, player1Symbol, player2Time);

        player2Start = chrono::system_clock::now();
        player2Agent.play();
        player2End = chrono::system_clock::now();
        chrono::duration<double> player2Duration = player2End - player2Start;
        double player2Seconds = player2Duration.count();
        player2Time -= player2Seconds;

        ReversiCommon::Move player2Move = readMove();
        PlayerMoveTime player2MoveTime(player2Symbol, player2Move, player2Seconds);
        gameLog.push_back(player2MoveTime);

        if (!player2Move.passMove()) {
            noOfPlayer2Moves += 1;
        }
        ReversiCommon::makeMove(player2Move, boardState, player2Symbol);

        // If both players don't have moves, we need to stop as the game has ended
        bothPlayersPassed = player1Move.passMove() && player2Move.passMove();
//         if (counter++ == 1) {
//             bothPlayersPassed = true;
//         }
    }
    replay();

    ReversiCommon::DiscsAndStableDiscs discsStable = ReversiCommon::numberOfDiscsAndStableDiscs(boardState);
    map<char, int> noOfDiscs = discsStable.numberOfDiscs;

    cout << "Field\t" << "Player1\t" << "Player2\t" << endl;
    cout << "Time\t" << player1Time << '\t' << player2Time << endl;
    cout << "Discs\t" << noOfDiscs[player1Symbol] << '\t' << noOfDiscs[player2Symbol] << endl;

    saveLog();
}

ReversiCommon::Move Server::readMove() {
    ifstream inputFile("output.txt");
    string moveString;
    inputFile >> moveString;
    inputFile.close();
    ReversiCommon::Move move = ReversiCommon::parseMove(moveString);
    return move;
}

void Server::replay() {
    WINDOW *win = initscr();
    noecho();
    refresh();
    char ch = '0';
    int currentIndex = -1;
    int gameSize = gameLog.size();
    initBoard();
    ReversiCommon::Move emptyMove(-2, -2, true);
    PlayerMoveTime emptyMoveTime('*', emptyMove, 0.0);

    while (ch != 'q') {
        if (currentIndex == -1) {
            showBoardCurses(emptyMoveTime, win, true);
        } else {
            PlayerMoveTime currentMoveTime = gameLog[currentIndex];
            showBoardCurses(currentMoveTime, win);
        }
        ch = getch();
        if (ch == 'f') {
            currentIndex = min(currentIndex + 1, gameSize - 1);
        } else if (ch == 'b') {
            currentIndex = max(-1, currentIndex - 1);
        }
        initBoard();
        for (int i = 0; i <= currentIndex; i++) {
            PlayerMoveTime playerMoveTime = gameLog[i];
            ReversiCommon::makeMove(playerMoveTime.move, boardState, playerMoveTime.player);
        }
    }
    endwin();
}

void Server::saveLog() {
    ofstream outputFile("gamelog.txt");

    for (int i = 0; i < gameLog.size(); i++) {
        PlayerMoveTime playerMoveTime = gameLog[i];
        char player = playerMoveTime.player;
        ReversiCommon::Move move = playerMoveTime.move;
        double time = playerMoveTime.time;

        outputFile << player << ' ' << ReversiCommon::formatMove(move) << ' ' << time << endl;
    }
    outputFile << endl;

    outputFile.close();
}


void Server::showBoardCurses(PlayerMoveTime &playerMoveTime, WINDOW *win, bool initState) {
    refresh();
    vector<ReversiCommon::Move> possibleMoves;
    if (!initState) {
        char opponentSymbol = playerMoveTime.player == 'X' ? 'O' : 'X';
        possibleMoves = ReversiCommon::validMoves(boardState, opponentSymbol);
        for (int i = 0; i < possibleMoves.size(); i++) {
            ReversiCommon::Move move = possibleMoves[i];
            boardState[move.x][move.y] = 'o';
        }
    }
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            char piece = boardState[i][j];
            if (piece == '*') {
                mvwaddch(win, i * 2, j * 2, piece | A_DIM);
            } else if (boardState[i][j] == 'o') {
                mvwaddch(win, i * 2, j * 2, piece | A_STANDOUT);
            } else if (!initState && (i == playerMoveTime.move.x && j == playerMoveTime.move.y)) {
                mvwaddch(win, i * 2, j * 2, piece | A_UNDERLINE);
            } else {
                mvwaddch(win, i * 2, j * 2, piece | A_BOLD);
            }
        }
    }

    if (!initState) {
        char player = playerMoveTime.player;
        string playerString = player == 'X'? "X" : "O";
        ReversiCommon::Move move = playerMoveTime.move;
        double time = playerMoveTime.time;
        string formattedString = playerString + " made move: " + ReversiCommon::formatMove(move) + " with time " + to_string(time);
        mvwaddstr(win, BOARD_SIZE * 2 + 2, 0, formattedString.c_str());
        for (int i = 0; i < possibleMoves.size(); i++) {
            ReversiCommon::Move move = possibleMoves[i];
            boardState[move.x][move.y] = '*';
        }
    }
    refresh();
}

int main(int argc, char *argv[]) {
    Server server;
    server.makePlay();
    return 0;
}
