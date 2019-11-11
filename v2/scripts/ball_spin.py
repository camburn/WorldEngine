import engine
import math
import random
import time


class V3:
    def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z


class EngineObject:
    """ Pythonic API for the C++ object """
    def __init__(self, c_obj):
        self._c_obj = c_obj

    @property
    def transform(self):
        return V3(*self._c_obj.get_transform())

    @transform.setter
    def transform(self, v3):
        self._c_obj.update_transform(v3.x, v3.y, v3.z)


def engine_call(func):
    def new_update(obj, dt):
        func(EngineObject(obj), dt)
    return new_update


@engine_call
def update(obj, dt):
    pos = obj.transform
    pos.x = math.sin(time.time()) * 3
    obj.transform = pos
    #obj.update_transform(x, y, z)
    yaw = math.sin(time.time() / 1000) * 180
    obj._c_obj.rotation = (0, yaw, 0)
