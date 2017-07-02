import code
import io
import sys
from rlcompleter import Completer


class Console(code.InteractiveConsole):
    """ Interactive console interface for use within the PyEngine """
    def __init__(self):
        super().__init__()
        self.stdout_buffer = io.StringIO()
        print('Initialised the Console in Python')
        self.last_result = False
        self.completer = Completer(self.locals)

    def reset_stdout_buffer(self):
        """ Clear the stdout buffer """
        self.stdout_buffer.close()
        self.stdout_buffer = io.StringIO()

    def write(self, data):
        print(data)

    def run_command(self, command):
        """ Run a python command and return all output """
        print('Running Command')
        # redirect stdout
        sys.stdout = self.stdout_buffer
        try:
            result = self.push(command)
        except Exception as err:
            print(f'This should never happen: {err}')
        self.last_result = result
        sys.stdout = sys.__stdout__
        if self.last_result:
            return '... ', ''
        buf = self.stdout_buffer.getvalue()
        self.reset_stdout_buffer()
        print('Returning', buf)
        return '>>> ', buf

    def _completions(self, partial_command):
        i = 0
        while True:
            option = self.completer.complete(partial_command, i)
            i += 1
            if option is None:
                break
            yield option


    def complete_command(self, partial_command):
        """
        Return a list of items that this partial_command could be completed to.
        """
        return tuple(self._completions(partial_command))
