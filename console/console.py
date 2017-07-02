import code
import io
import sys


class Console(code.InteractiveConsole):
    """  """
    def __init__(self):
        super().__init__()
        self.stdout_buffer = io.StringIO()
        print('Initialised the Console in Python')
        self.last_result = False

    def reset_stdout_buffer(self):
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
        print('Returning',  buf)
        return '>>> ', buf