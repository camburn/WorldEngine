""" Template Python Script """
import math
import time
from scripts.engine import engine_call

@engine_call
def update(obj, delta_time):
    """ Update is called every frame. """
    pos = obj.transform
    pos.x = math.sin(time.time()) * 3
    obj.transform = pos
