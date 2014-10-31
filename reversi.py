# import time
import curses

POS_WEIGHTS_HW = [
    [99, -8, 8, 6, 6, 8, -8, 99],
    [-8, -24, -4, -3, -3, -4, -24, -8],
    [8, -4, 7, 4, 4, 7, -4, 8],
    [6, -3, 4, 0, 0, 4, -3, 6],
    [6, -3, 4, 0, 0, 4, -3, 6],
    [8, -4, 7, 4, 4, 7, -4, 8],
    [-8, -24, -4, -3, -3, -4, -24, -8],
    [99, -8, 8, 6, 6, 8, -8, 99]
]
COLUMN_NAMES = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h']

BOARD_SIZE = 8
INF_NEG, INF_POS = -float('inf'), float('inf')
DIRECTIONS = [
    [-1, -1], [-1, 0], [-1, 1],
    [0, -1], [0, 1],  # [0, 0],
    [1, -1], [1, 0], [1, 1]
]


class ReversiAgent:
    def __init__(self, params, output=True):
        """
        Plays Reversi
        :param params: Contains parameters necessary to proceed with
         game
        """
        self.cutoff_depth = params['cutoff_depth']
        self.current_state = params['current_state']
        self.max_player = params['player']
        self.min_player = params['opponent']
        self.traverse_log = list()
        self.task = params['task']
        self.curses_output = output

        if self.curses_output:
            # Init some curses settings
            self.scr = curses.initscr()
            curses.noecho()

    def common_move_check(self, i, j, check_for_star=True):
        within_limits = 0 <= j < BOARD_SIZE and 0 <= i < BOARD_SIZE
        if check_for_star:
            return within_limits and self.current_state[i][j] != '*'
        else:
            return within_limits

    def evaluate_score(self):
        max_player_score = 0
        min_player_score = 0
        for i in xrange(BOARD_SIZE):
            for j in xrange(BOARD_SIZE):
                if self.current_state[i][j] == self.min_player:
                    min_player_score += POS_WEIGHTS_HW[i][j]
                elif self.current_state[i][j] == self.max_player:
                    max_player_score += POS_WEIGHTS_HW[i][j]
        return max_player_score - min_player_score

    def play(self):
        alpha_beta_prune = False
        if self.task == 1:
            # Greedy
            if self.curses_output:
                print "Running Greedy"
            self.cutoff_depth = 1
        elif self.task == 2:
            # MiniMax
            if self.curses_output:
                print "Running MiniMax"
            pass
        elif self.task == 3:
            # AlphaBeta
            if self.curses_output:
                print "Running AlphaBetaPruning"
            alpha_beta_prune = True

        # self.print_board()

        alpha, beta = INF_NEG, INF_POS
        (val, move) = self.max_play(0, alpha_beta_prune, alpha, beta, (None, None))

        if self.curses_output:
            curses.endwin()
            print val
        if move is not None:
            (i, j) = move
            self.make_move(i, j, self.max_player)
            if self.curses_output:
                self.print_board()
        self.write_output()
        return (val, move)

    @staticmethod
    def format_move(x, y):
        if x is None or y is None:
            return "root"
        return "{0}{1}".format(COLUMN_NAMES[y], x + 1)

    @staticmethod
    def format_infinity(*args):
        infinity_list = list()
        for arg in args:
            if arg == INF_NEG:
                infinity_list.append("-Infinity")
            elif arg == INF_POS:
                infinity_list.append("Infinity")
            else:
                infinity_list.append(arg)
        return infinity_list

    def print_board(self):
        for i in xrange(BOARD_SIZE):
            for j in xrange(BOARD_SIZE):
                print self.current_state[i][j],
            print
        print

    def write_output(self):
        try:
            output_file = open("output.txt", "w")
            if self.task == 1:
                # Greedy; no need to write task log
                self.traverse_log = list()
            for i in xrange(BOARD_SIZE):
                output_file.write(' '.join(self.current_state[i]))
                output_file.write('\n')
            for i in xrange(len(self.traverse_log)):
                ((x, y), depth, value, alpha, beta) = self.traverse_log[i]
                if depth == 0:
                    (x, y) = (None, None)
                move = self.format_move(x, y)
                (depth, value, alpha, beta) = self.format_infinity(depth, value, alpha, beta)
                print move, depth, value
                if self.task == 2:
                    # MiniMax, write only 3 values
                    output_file.write("{0},{1},{2}".format(move, depth, value))
                elif self.task == 3:
                    # AlphaBeta, write 5 values
                    output_file.write("{0},{1},{2},{3},{4}".format(move, depth, value, alpha, beta))
                output_file.write('\n')
            output_file.close()
        except IOError as e:
            print "Error writing output"
            print e.message

    def make_move(self, x, y, player):
        flips = dict()
        self.current_state[x][y] = player
        for d in xrange(len(DIRECTIONS)):
            (delta_i, delta_j) = DIRECTIONS[d]
            i, j = x + delta_i, y + delta_j
            found_player = False
            while self.common_move_check(i, j) and self.current_state[i][j] != player:
                # First check if flips are possible in this direction
                i += delta_i
                j += delta_j
                if self.common_move_check(i, j) and self.current_state[i][j] == player:
                    found_player = True
            if (i, j) != (x + delta_i, y + delta_j) and found_player:
                # Then, if flips are possible, make the flips
                a, b = x + delta_i, y + delta_j
                while (a, b) != (i, j):
                    self.current_state[a][b] = player
                    (a, b) = (a + delta_i, b + delta_j)
                flips[d] = (i, j)
        #print "Make move: ", (x, y), flips
        #self.print_board()
        #time.sleep(1)
        self.print_board_curses(list(), ["Make move:", self.format_move(x, y)])
        return flips

    def undo_move(self, x, y, player, flips):
        self.current_state[x][y] = '*'
        if player == 'X':
            opponent = 'O'
        else:
            opponent = 'X'
        for d in flips:
            (delta_i, delta_j) = DIRECTIONS[d]
            i, j = x + delta_i, y + delta_j
            while self.common_move_check(i, j) and (i, j) != flips[d]:
                self.current_state[i][j] = opponent
                i += delta_i
                j += delta_j
        #print "Undo move: ", (x, y), flips
        #self.print_board()
        #time.sleep(1)
        self.print_board_curses(list(), ["Undo move:", self.format_move(x, y)])

    def max_play(self, depth, prune, alpha, beta, move):
        valid_moves = self.valid_moves(self.max_player)
        (m1, m2) = move
        if self.terminal_test(depth, valid_moves):
            self.traverse_log.append([(m1, m2), depth, self.evaluate_score(), alpha, beta])
            return (self.evaluate_score(), None)

        value = INF_NEG
        best_move = None
        self.traverse_log.append([(m1, m2), depth, value, alpha, beta])
        for (i, j) in valid_moves:
            flips = self.make_move(i, j, self.max_player)
            min_value, next_move = self.min_play(depth + 1, prune, alpha, beta, (i, j))
            if min_value > value:
                best_move = (i, j)
                value = min_value
            elif min_value == value:
                order = sorted([(i, j), best_move])
                if order.index((i, j)) == 0:
                    best_move = (i, j)
                    value = min_value
            self.undo_move(i, j, self.max_player, flips)
            if prune:
                if value >= beta:
                    self.traverse_log.append([(m1, m2), depth, value, alpha, beta])
                    return (value, best_move)
                alpha = max(alpha, value)
            self.traverse_log.append([(m1, m2), depth, value, alpha, beta])
        # self.traverse_log.append([(m1, m2), depth, value, alpha, beta])
        return (value, best_move)

    def min_play(self, depth, prune, alpha, beta, move):
        valid_moves = self.valid_moves(self.min_player)
        (m1, m2) = move
        if self.terminal_test(depth, valid_moves):
            (i, j) = move
            self.traverse_log.append([(m1, m2), depth, self.evaluate_score(), alpha, beta])
            return (self.evaluate_score(), None)

        value = INF_POS
        best_move = None
        self.traverse_log.append([(m1, m2), depth, value, alpha, beta])
        for (i, j) in valid_moves:
            flips = self.make_move(i, j, self.min_player)
            max_value, next_move = self.max_play(depth + 1, prune, alpha, beta, (i, j))
            if max_value < value:
                best_move = (i, j)
                value = max_value
            elif max_value == value:
                order = sorted([(i, j), best_move])
                if order.index((i, j)) == 0:
                    best_move = (i, j)
                    value = max_value
            self.undo_move(i, j, self.min_player, flips)
            if prune:
                if value <= alpha:
                    self.traverse_log.append([(m1, m2), depth, value, alpha, beta])
                    return (value, best_move)
                beta = min(beta, value)
            self.traverse_log.append([(m1, m2), depth, value, alpha, beta])
        # self.traverse_log.append([(m1, m2), depth, value, alpha, beta])
        return (value, best_move)

    def print_board_curses(self, valid_moves_list, args=[]):
        if not self.curses_output:
            return
        self.scr.refresh()
        for c in xrange(BOARD_SIZE):
            self.scr.addstr(0, c * 2 + 4, COLUMN_NAMES[c], curses.A_BOLD)
            self.scr.addstr(c + 1, 0, str(c + 1), curses.A_BOLD)
        for i in xrange(BOARD_SIZE):
            for j in xrange(BOARD_SIZE):
                if self.current_state[i][j] in ['x', 'o']:
                    self.scr.addstr(i + 1, j * 2 + 4, self.current_state[i][j], curses.A_STANDOUT)
                elif self.current_state[i][j] == '*':
                    self.scr.addstr(i + 1, j * 2 + 4, '.', curses.A_INVIS)
                else:
                    self.scr.addstr(i + 1, j * 2 + 4, self.current_state[i][j], curses.A_BOLD)
        for i in xrange(len(valid_moves_list)):
            (x, y) = valid_moves_list[i]
            self.scr.addstr(BOARD_SIZE * 2 + 2, i * 3, self.format_move(x, y))
        l = 0
        for i in xrange(len(args)):
            self.scr.addstr(BOARD_SIZE * 2 + 3, l, args[i])
            l += len(args[i]) + 2
        self.scr.refresh()
        self.scr.getch()

    def valid_moves(self, player):
        valid_moves_list = list()
        for x in xrange(BOARD_SIZE):
            for y in xrange(BOARD_SIZE):
                if self.current_state[x][y] != '*':
                    continue
                for (delta_i, delta_j) in DIRECTIONS:
                    (i, j) = (x + delta_i, y + delta_j)
                    while self.common_move_check(i, j, False) and self.current_state[i][j] not in [player, '*']:
                        (i, j) = (i + delta_i, j + delta_j)
                    if (i, j) != (x + delta_i, y + delta_j) and self.common_move_check(i, j, False) and\
                            self.current_state[i][j] == player:
                        valid_moves_list.append((x, y))
                        break

        valid_moves_list = sorted(valid_moves_list)
        for (x, y) in valid_moves_list:
            self.current_state[x][y] = player.lower()
        self.print_board_curses(valid_moves_list)

        for (x, y) in valid_moves_list:
            self.current_state[x][y] = '*'
            # print (x, y), ":", self.format_move(x, y)
        # print
        return valid_moves_list

    def terminal_test(self, depth, valid_moves):
        return not (depth < self.cutoff_depth and valid_moves is not None and len(valid_moves) > 0)
