#ifndef REVERSIHWAGENT_H
#define REVERSIHWAGENT_H

#include "reversicommon.h"

#include <string>
#include <vector>

using namespace std;

class ReversiHWAgent : public reversi::ReversiCommon {
public:
    ReversiHWAgent(int cutoffDepth, vector<vector<char> > &currentState, char player, char opponent, int task);
    void play();

private:
    int cutoffDepth;

    MoveValue minMax(int depth, int alpha, int beta, Move move, bool max);

    void writeOutput();
};

#endif // REVERSIHWAGENT_H
