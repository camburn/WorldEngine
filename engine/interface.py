"""
Interface for the python methods intended to be called from the Engine
"""
import time
import engine_console


ON_INIT_FUNCTIONS = []
ON_FRAME_FUNCTIONS = []


def on_init():
    print("Initialising from Python")
    for function in ON_INIT_FUNCTIONS:
        function()


def on_frame(frame_time):
    engine_console.write_line("Processing Frame {}".format(time.time()))
    for function in ON_FRAME_FUNCTIONS:
        function(frame_time)
    
