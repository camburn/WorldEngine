"""
Interface for the python methods intended to be called from the Engine
"""
import time
import engine_console
import manager
import math

ON_INIT_FUNCTIONS = []
ON_FRAME_FUNCTIONS = []

DATA_STORE = {"total_time" : 0}

def register_on_frame(function):
    ON_FRAME_FUNCTIONS.append(function)

def on_init():
    print("Initialising from Python")
    for function in ON_INIT_FUNCTIONS:
        function()


def on_frame(frame_time):
    #engine_console.write_line("Processing Frame {}".format(time.time()))
    for function in ON_FRAME_FUNCTIONS:
        try:
            function(frame_time)
        except Exception as err:
            engine_console.write_line("On frame exception")
            engine_console.write_line(str(err))

@register_on_frame
def move_around(frame_time):
    DATA_STORE['total_time'] += frame_time
    x = math.sin(DATA_STORE['total_time']) * 3
    manager.set_primitive_position(1, x, 0.5, 0)
