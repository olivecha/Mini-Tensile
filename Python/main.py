import logging
import time
from threading_class import FastCounter

# We use logging to print information, it will allow us to see
# which thread is sending which message
logging.basicConfig(level=logging.DEBUG, format='(%(threadName)-9s) %(message)s', )
# We init the FastCounter class
threaded_class = FastCounter()
# We start the FastCounter class, the class is now running
# the run function in a thread
threaded_class.start()
# We perform a 5 seconds blocking action
time.sleep(5)
# The status variable of FastCounter is switched to False
threaded_class.status = False
