import serial
from collections import deque
import matplotlib.animation as animation
import matplotlib.pyplot as plt

strPort = "/dev/cu.usbserial-1420"
baud = 115200
Arduino = serial.Serial(strPort, baud)
maxLen = 300
force = deque([0.0] * maxLen)
position = deque([0.0] * maxLen)

fig = plt.figure()
ax = plt.axes(xlim=(0, 3), ylim=(0, 300))
plt.xlabel('allongement')
plt.ylabel('force')
line, = ax.plot([], [], lw=2, c='r')


def init():
    line.set_data([], [])
    return line,


# animation function.  This is called sequentially
def animate1(i, f, p):
    line1 = Arduino.readline()
    data = [float(val) for val in line1.split()]
    if len(data) == 2:
        f.append(data[1])
        f.popleft()
        p.append(data[0])
        p.popleft()
    line.set_data(p, f)
    return line,


anim = animation.FuncAnimation(fig, animate1, fargs=(force, position), init_func=init, interval=100, blit=True)
