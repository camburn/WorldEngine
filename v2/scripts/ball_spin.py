import engine
import math
import time

def update(dt):
    #x, y, z = engine.get_object_transform()
    x = math.sin(time.time()) * 5
    #engine.update_object_transform(x, y, z)
    yaw = math.sin(time.time() / 1000) * 180
    engine.update_object_rotation(0, yaw, 0)
