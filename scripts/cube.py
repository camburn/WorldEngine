import engine_input
""" Template Python Script """
from scripts.engine import engine_call

@engine_call
def update(obj, delta_time):
    """ Update is called every frame. """
    if engine_input.get_mouse_button(0):
        print('LEFT MOUSE WAS CLICKED')
    elif engine_input.get_mouse_button(1):
        print('RIGHT MOUSE WAS CLICKED')
    elif engine_input.get_mouse_button(2):
        print('MIDDLE MOUSE WAS CLICKED')

@engine_call
def on_click(obj):
    """ on_click is called when an object is clicked on with a collider """
    print("This object was clicked!")
    pos = obj.transform
    pos.x = pos.x + 1
    obj.transform = pos

