#include <iostream>
#include <fstream>

#include "reversihwagent.h"
#include "reversicompetitionagent.h"

using namespace std;

int main(int argc, char **argv) {
    ifstream inputFile("input.txt");
    if(!inputFile.is_open()) {
        cout << "Couldn't open file: input.txt" << endl;
    }
    int task;
    char player;
    vector<vector<char> > board;
    double cpuTime;
    int cutoffDepth;

    inputFile >> task;
    inputFile >> player;

    char opponent = (player == 'X'? 'O': 'X');

    if (1 <= task <= 3) {
        inputFile >> cutoffDepth;
    } else {
        inputFile >> cpuTime;
    }

    for (int i = 0; i < BOARD_SIZE; i++) {
        vector<char> row;
        char ch;
        for (int j = 0; j < BOARD_SIZE; j++) {
            inputFile >> ch;
            row.push_back(ch);
        }
        board.push_back(row);
    }

    if (1 <= task && task <= 3) {
    } else if (task == 4) {
        // Competition
        ReversiCompetitionAgent reversiAgent(board, player, opponent, cpuTime);
        reversiAgent.play();
    }

    return 0;
}
