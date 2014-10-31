#ifndef SERVER_H
#define SERVER_H

#include "reversicommon.h"

#include <ncurses.h>

using namespace std;
using namespace reversi;

char INIT_BOARD_STATE[BOARD_SIZE][BOARD_SIZE] = {
    {'*', '*', '*', '*', '*', '*', '*', '*'},
    {'*', '*', '*', '*', '*', '*', '*', '*'},
    {'*', '*', '*', '*', '*', '*', '*', '*'},
    {'*', '*', '*', 'X', 'O', '*', '*', '*'},
    {'*', '*', '*', 'O', 'X', '*', '*', '*'},
    {'*', '*', '*', '*', '*', '*', '*', '*'},
    {'*', '*', '*', '*', '*', '*', '*', '*'},
    {'*', '*', '*', '*', '*', '*', '*', '*'}
};

class Server
{
public:
    Server();
    void makePlay();

private:
    struct PlayerMoveTime {
        char player;
        ReversiCommon::Move move;
        double time;
        PlayerMoveTime(char player, ReversiCommon::Move &move, double time):
                player(player), move(move), time(time) {
        }
    };

    int task;

    vector<vector<char> > boardState;

    vector<PlayerMoveTime> gameLog;

    vector<vector<char> > cloneBoard();

    /**
     * Generate input for the next player as input.txt
     */
    void generateInput(int task, double cpuTime, char player);

    /**
     * Read move from file, parse it and return a valid mvoe
     */
    ReversiCommon::Move readMove();

    /**
     * Replay the whole game after it has ended
     */
    void replay();

    /**
     * Initialize the game state to the default start state
     */
    void initBoard();

    /**
     * Save the game log for scientific purposes
     */
    void saveLog();

    /**
     * Display the current state with the move and time taken for the move
     * for the current player
     */
    void showBoardCurses(Server::PlayerMoveTime& playerMoveTime, WINDOW* win, bool initState=false);
};

#endif // SERVER_H
