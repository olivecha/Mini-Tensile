from collections import deque
import argparse
import matplotlib.animation as animation
import matplotlib.pyplot as plt
import serial


# plot class
class AnalogPlot:
    def __init__(self, maxLen, strPort):
        # open serial port
        self.ser = serial.Serial(strPort, 115200)
        self.ax = deque([0.0] * maxLen)
        self.ay = deque([0.0] * maxLen)
        self.maxLen = maxLen

    # add to buffer
    def addToBuf(self, buf, val):
        if len(buf) < self.maxLen:
            buf.append(val)
        else:
            buf.pop()
            buf.appendleft(val)

    # add data
    def add(self, data):
        assert (len(data) == 2)
        self.addToBuf(self.ax, data[0])
        self.addToBuf(self.ay, data[1])

    # update plot
    def update(self, frameNum, a0, a1):

        line = self.ser.readline()
        print(line)
        data = [float(val) for val in line.split()]
        # print data
        if len(data) == 2:
            self.add(data)
            print(data)
            a0.set_data(range(self.maxLen), self.ax)
            a1.set_data(range(self.maxLen), self.ay)

        return a0,

        # clean up

    def close(self):
        # close serial
        self.ser.flush()
        self.ser.close()

    # main() function


def main():
    # create parser
    parser = argparse.ArgumentParser(description="LDR serial")

    # add expected arguments
    parser.add_argument('--port', dest='port', required=True)

    # parse args
    #args = parser.parse_args(['--port'])

    strPort = "/dev/cu.usbserial-1420"
    #strPort = args.port

    print('reading from serial port %s...' % strPort)

    # plot parameters
    analogPlot = AnalogPlot(100, strPort)

    print('plotting data...')

    # set up animation
    fig = plt.figure()
    ax = plt.axes()
    a0, = ax.plot([], [])
    a1, = ax.plot([], [])
    anim = animation.FuncAnimation(fig, analogPlot.update, fargs=(a0, a1), interval=5)

    # show plot
    plt.show()

    # clean up
    #analogPlot.close()

    print('exiting.')


# call main
if __name__ == '__main__':
    main()