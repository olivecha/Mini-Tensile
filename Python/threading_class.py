import logging
import threading


# We create a class that inherits from the python threading.Thread class
class FastCounter(threading.Thread):

    def __init__(self):
        # The following line executes the init code of the threading.Thread class
        super().__init__()
        logging.debug('Threaded class just started.')
        # The class has a counter variable
        self.count = 0
        # And a boolean variable called status
        self.status = True

    # This function must be called "run", it is the main function of this thread
    def run(self):
        # While the variable status is True, we do this
        # When status becomes False, the run() function will end
        while self.status is True:
            # If the counter is divisible by 1000000 we print the following line
            if int(self.count) % 1000000 == 0:
                logging.debug("Count is at: " + str(self.count))
            # At the end of the loop we increase the counter
            self.count = self.count + 1

    # This function must be called stop, it will be executed when the function run is done
    def stop(self):
        logging.debug("Closing threaded class...")
        # join is a function from threading.Thread that closes the thread
        self.join()
