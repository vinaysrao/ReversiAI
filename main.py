import reversi
import play


def main():
    try:
        input_file = open("input.txt")
    except IOError as e:
        print "Unable to open input.txt"
        return
    try:
        task = int(input_file.readline().strip())
        player = input_file.readline().strip()
        if player == 'X':
            opponent = 'O'
        else:
            opponent = 'X'
        cutoff_depth = int(input_file.readline().strip())
        current_state = []
        for i in xrange(reversi.BOARD_SIZE):
            current_state.append(list())
            board_row = input_file.readline().strip()
            for c in board_row.split():
                current_state[i].extend(c)
        input_file.close()
    except Exception as e:
        print "Unable to read parameters from file"
        print e.message
        return
    params = dict()
    params['cutoff_depth'] = cutoff_depth
    params['current_state'] = current_state
    params['opponent'] = opponent
    params['player'] = player
    params['task'] = task
    reversi_agent = reversi.ReversiAgent(params, output=False)
    reversi_agent.play()

if __name__ == "__main__":
    main()
