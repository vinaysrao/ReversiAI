import random
import reversi
import curses
import copy
import time

DIRECTIONS = [
    [-1, -1], [-1, 0], [-1, 1],
    [0, -1], [0, 1],  # [0, 0],
    [1, -1], [1, 0], [1, 1]
]
PLAYERS = ['X', 'O']
COLUMN_NAMES = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h']
BOARD_SIZE = 8
START_STATE = [
    ['*', '*', '*', '*', '*', '*', '*', '*'],
    ['*', '*', '*', '*', '*', '*', '*', '*'],
    ['*', '*', '*', '*', '*', '*', '*', '*'],
    ['*', '*', '*', 'X', 'O', '*', '*', '*'],
    ['*', '*', '*', 'O', 'X', '*', '*', '*'],
    ['*', '*', '*', '*', '*', '*', '*', '*'],
    ['*', '*', '*', '*', '*', '*', '*', '*'],
    ['*', '*', '*', '*', '*', '*', '*', '*']
]


class Play:
    def __init__(self):
        self.scr = curses.initscr()
        curses.noecho()
        random.seed(time.clock())
        self.player1_depth = random.randint(1, 4)
        self.player2_depth = random.randint(1, 4)
        self.task = 3
        self.player1 = PLAYERS[0]
        self.player2 = PLAYERS[1]
        self.current_state = copy.deepcopy(START_STATE)
        self.moves_list = list()

    def common_move_check(self, i, j, check_for_star=True, board=None):
        if board is None:
            board = self.current_state
        within_limits = 0 <= j < BOARD_SIZE and 0 <= i < BOARD_SIZE
        if check_for_star:
            return within_limits and board[i][j] != '*'
        else:
            return within_limits

    def generate_input(self, player, depth):
        params = dict()
        params['cutoff_depth'] = depth
        params['current_state'] = copy.deepcopy(self.current_state)
        if player == 'X':
            opponent = 'O'
        else:
            opponent = 'X'
        params['opponent'] = opponent
        params['player'] = player
        params['task'] = self.task
        return params

    @staticmethod
    def format_move(x, y):
        if x is None or y is None:
            return "root"
        return "{0}{1}".format(COLUMN_NAMES[y], x + 1)

    def evaluate_score(self, board=None, just_count=False):
        if board is None:
            board = self.current_state
        player1_score = 0
        player2_score = 0
        for i in xrange(BOARD_SIZE):
            for j in xrange(BOARD_SIZE):
                if board[i][j] == self.player1:
                    if just_count:
                        player1_score += 1
                    else:
                        player1_score += reversi.POS_WEIGHTS_HW[i][j]
                elif board[i][j] == self.player2:
                    if just_count:
                        player2_score += 1
                    else:
                        player2_score += reversi.POS_WEIGHTS_HW[i][j]
        return player1_score, player2_score

    def play(self):
        print "Computing full game with X:{0} and Y:{1}".format(self.player1_depth, self.player2_depth)
        while 1:
            # Player1 goes first
            player1_params = self.generate_input(self.player1, self.player1_depth)
            player1 = reversi.ReversiAgent(player1_params, output=False)
            (player1_value, player1_move) = player1.play()
            self.make_move(self.player1, player1_move)
            if player1_move is not None:
                self.moves_list.append((self.player1, player1_move))
                # (x, y) = player1_move
                # self.print_board_curses(args=["%s made move" % self.player1, self.format_move(x, y)])

            #Then goes Player2
            player2_params = self.generate_input(self.player2, self.player2_depth)
            player2 = reversi.ReversiAgent(player2_params, output=False)
            (player2_value, player2_move) = player2.play()
            self.make_move(self.player2, player2_move)
            if player2_move is not None:
                self.moves_list.append((self.player2, player2_move))
            #     (x, y) = player2_move
            #     self.print_board_curses(args=["%s made move" % self.player2, self.format_move(x, y)])

            if player1_move is None and player2_move is None:
                break
            # if player1_move is not None:
            #     (x1, y1) = player1_move
            # else:
            #     (x1, y1) = (0, 0)
            # if player2_move is not None:
            #     (x2, y2) = player2_move
            # else:
            #     (x2, y2) = (0, 0)
            # print self.format_move(x1, y1), self.format_move(x2, y2)
        (score_x, score_y) = self.evaluate_score(just_count=True)
        print score_x, score_y
        self.replay()

    def replay(self):
        game_length = len(self.moves_list)
        current_index = 0
        ch = None
        while ch != ord('q'):
            board = copy.deepcopy(START_STATE)
            for i in xrange(current_index):
                (player, move) = self.moves_list[i]
                self.make_move(player, move, board=board)
            (player, move) = self.moves_list[current_index]
            (x, y) = move
            ch = self.print_board_curses(args=["%s to move" % player, self.format_move(x, y),
                                               "X:%s,Y:%s" % self.evaluate_score(board)], board=board)
            if ch == ord('f'):
                current_index = min(current_index + 1, game_length - 1)
            if ch == ord('b'):
                current_index = max(0, current_index - 1)
        curses.endwin()

    def make_move(self, player, move, board=None):
        if board is None:
            board = self.current_state
        if move is None:
            return
        (x, y) = move
        flips = dict()
        board[x][y] = player
        for d in xrange(len(DIRECTIONS)):
            (delta_i, delta_j) = DIRECTIONS[d]
            i, j = x + delta_i, y + delta_j
            found_player = False
            while self.common_move_check(i, j, board=board) and board[i][j] != player:
                # First check if flips are possible in this direction
                i += delta_i
                j += delta_j
                if self.common_move_check(i, j, board=board) and board[i][j] == player:
                    found_player = True
            if (i, j) != (x + delta_i, y + delta_j) and found_player:
                # Then, if flips are possible, make the flips
                a, b = x + delta_i, y + delta_j
                while (a, b) != (i, j):
                    board[a][b] = player
                    (a, b) = (a + delta_i, b + delta_j)
                flips[d] = (i, j)
        #print "Make move: ", (x, y), flips
        #self.print_board()
        #time.sleep(1)
        return flips

    def print_board_curses(self, valid_moves_list=list(), args=list(), board=None):
        if board is None:
            board = self.current_state
        self.scr.refresh()
        for c in xrange(BOARD_SIZE):
            self.scr.addstr(0, c * 2 + 4, COLUMN_NAMES[c], curses.A_BOLD)
            self.scr.addstr(c + 1, 0, str(c + 1), curses.A_BOLD)
        for i in xrange(BOARD_SIZE):
            for j in xrange(BOARD_SIZE):
                if board[i][j] in ['x', 'o']:
                    self.scr.addstr(i + 1, j * 2 + 4, board[i][j], curses.A_STANDOUT)
                elif board[i][j] == '*':
                    self.scr.addstr(i + 1, j * 2 + 4, '.', curses.A_INVIS)
                else:
                    self.scr.addstr(i + 1, j * 2 + 4, board[i][j], curses.A_BOLD)
                if board[i][j] == 'X':
                    self.scr.addstr(i + 1, BOARD_SIZE * 3 + (j * 4) + 4, str(reversi.POS_WEIGHTS_HW[i][j]))
                    self.scr.addstr(i + 1, BOARD_SIZE * 8 + (j * 4) + 4, ".   ")
                elif board[i][j] == 'O':
                    self.scr.addstr(i + 1, BOARD_SIZE * 8 + (j * 4) + 4, str(reversi.POS_WEIGHTS_HW[i][j]))
                    self.scr.addstr(i + 1, BOARD_SIZE * 3 + (j * 4) + 4, ".   ")
                else:
                    self.scr.addstr(i + 1, BOARD_SIZE * 3 + (j * 4) + 4, ".   ")
                    self.scr.addstr(i + 1, BOARD_SIZE * 8 + (j * 4) + 4, ".   ")
        for i in xrange(len(valid_moves_list)):
            (x, y) = valid_moves_list[i]
            self.scr.addstr(BOARD_SIZE * 2 + 2, i * 3, self.format_move(x, y))
        l = 0
        for i in xrange(len(args)):
            self.scr.addstr(BOARD_SIZE * 2 + 3, l, args[i])
            l += len(args[i]) + 2
        self.scr.refresh()
        return self.scr.getch()

if __name__ == "__main__":
    p = Play()
    p.play()