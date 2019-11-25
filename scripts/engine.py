
class V3:
    def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z

    def __repr__(self):
        return f"V3({self.x},{self.y},{self.z})"


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
