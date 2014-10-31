import matplotlib.pyplot as plt

playerTimes = dict()
playerTimes['X'] = list()
playerTimes['O'] = list()

for line in open("gamelog.txt"):
    l = line.strip()
    if l == '':
        continue
    (player, move, time) = l.split(' ')
    time = float(time)
    playerTimes[player].append(time)

player1Times = playerTimes['X']
player2Times = playerTimes['O']

player1_plot = plt.plot(xrange(len(player1Times)), player1Times, color='r', label='Player(X)')
player2_plot = plt.plot(xrange(len(player2Times)), player2Times, color='b', label='Player(O)')

plt.legend()

plt.show()
