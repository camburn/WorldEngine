import time
import math
from scripts.engine import engine_call

@engine_call
def update(obj, dt):
    yaw = math.sin(time.time() / 500) * 180
    obj._c_obj.rotation = (0, yaw, 0)
