import logging, sys
import time
# We import the queue class from the standard library
import queue
from threading_class import FastCounter

# We use logging to print information, it will allow us to see
# which thread is sending which message
logging.basicConfig(level=logging.DEBUG,
                    format='(%(threadName)-9s) %(message)s',)
# We defined a queue that will be used to transfer data from the threaded class to the main program
threaded_q = queue.Queue()
# We init the FastCounter class and feed it the queue as an arg
threaded_class = FastCounter(args = (threaded_q))
# We start the FastCounter class, the class is now running
# the run function in a thread
threaded_class.start()
# We perform a 1 seconds blocking action
time.sleep(1)
# We display the values from the queue for a 1 seconds, then exit the program
now = time.time()
while True:
    logging.debug(threaded_q.get())
    if time.time() - now > 1:
        break
# The status variable of FastCounter is switched to False
threaded_class.status = False