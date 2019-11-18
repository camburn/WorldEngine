import math
import random
import time

from scripts.engine import engine_call

@engine_call
def update(obj, dt):
    pos = obj.transform
    pos.x = math.sin(time.time()) * 3
    obj.transform = pos
    #obj.update_transform(x, y, z)
    yaw = math.sin(time.time() / 100) * 180
    obj._c_obj.rotation = (0, yaw, 0)
