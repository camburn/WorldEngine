""" Template Python Script """
import math
import time
from scripts.engine import engine_call

@engine_call
def update(obj, delta_time):
    """ Update is called every frame. """
    pos = obj.transform
    pos.x = math.cos(time.time()) * 5
    pos.y = (math.sin(time.time()) * 2)
    if (pos.y < 0.0):
        obj._c_obj.set_enabled(False)
        obj.enabled = False
    else:
        obj._c_obj.set_enabled(True)
        obj.enabled = True
    obj.transform = pos
