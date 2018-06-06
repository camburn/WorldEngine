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

def register_on_init(function):
    ON_INIT_FUNCTIONS.append(function)

def on_init():
    for function in ON_INIT_FUNCTIONS:
        try:
            function()
        except Exception as err:
            engine_console.write_line("On frame exception")
            engine_console.write_line(str(err))


def on_frame(frame_time):
    for function in ON_FRAME_FUNCTIONS:
        try:
            function(frame_time)
        except Exception as err:
            engine_console.write_line("On frame exception")
            engine_console.write_line(str(err))


@register_on_init
def create_mountain():
    manager.create_primitive('Cube', 'metal_box', 0, 0.5, -5)
    manager.create_primitive('Cube', 'metal_box', 1, 0.5, -5)
    manager.create_primitive('Cube', 'metal_box', 2, 0.5, -5)
    manager.create_primitive('Cube', 'metal_box', 0.5, 1.5, -5)
    manager.create_primitive('Cube', 'metal_box', 1.5, 1.5, -5)
    manager.create_primitive('Cube', 'metal_box', 1.0, 2.5, -5)
    
    DATA_STORE['box_a'] = manager.create_primitive('Cube', 'metal_box', 3.0, 0.5, 3)


@register_on_frame
def move_around(frame_time):
    DATA_STORE['total_time'] += frame_time
    x = math.sin(DATA_STORE['total_time']) * 3
    manager.set_primitive_position(1, x, 0.5, 0)
    manager.set_primitive_position(DATA_STORE['box_a'], x, 0.5, 3)
