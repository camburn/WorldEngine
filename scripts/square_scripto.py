""" Template Python Script """
import math
import time
from scripts.engine import engine_call

@engine_call
def update(obj, delta_time):
    """ Update is called every frame. """
    yaw = math.sin(time.time() / 1000) * 180
    obj._c_obj.rotation = (0, yaw, 0)
