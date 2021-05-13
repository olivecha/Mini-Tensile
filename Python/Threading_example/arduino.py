import threading
import serial, time
import logging, sys
import config


class Arduino(threading.Thread):

    def __init__(self, args=(), kwargs=None, baudrate=115200, port="/dev/ttyACM0"):
        super().__init__()
        self.q = args
        self.args = args
        self.port = port
        self.baudrate = baudrate
        self.kwargs = kwargs
        self.plugged = False
        self.test_arduino()

    def run(self):
        while True:
            if config.run == False:
                self.stop()
                break
            if self.plugged == True:
                data_d = self.read_line()
                with self.q.mutex:
                    self.q.queue.clear()
                self.q.put([data_d])
                self.arduino.flushInput()
            else:
                self.test_arduino()

    def join(self):
        self.stop()
        sys.exit(1)

    def stop(self):
        logging.debug("Closing Arduino thread...")
        self.arduino.close()

    def read_line(self):
        results = []
        save_next = False
        while True:
            val = self.arduino.readline()
            if '\\n' in str(val) and save_next == True:
                try:
                    val = val.strip().decode("utf-8")
                    results.append(val)
                    return "".join(results)
                except:
                    pass
            elif '\\n' in str(val) and save_next == False:
                save_next = True
            elif save_next == True:
                try:
                    val = val.strip().decode("utf-8")
                    results.append(val)
                except UnicodeDecodeError:
                    pass

    def test_arduino(self):
        try:
            self.arduino = serial.Serial(self.port, self.baudrate, timeout = None)
            self.plugged = True
            logging.debug("Successfully connected to the Arduino port.")
        except serial.serialutil.SerialException as e:
            logging.warning("An ADC channel is specified in the deck, but I can't access it: " + str(e))
            self.plugged = False

    def __exit__(self, exc_type, exc_value, traceback) :
        self.arduino.close()