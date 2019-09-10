#pragma once
#ifndef _EVENT_HPP
#define _EVENT_HPP
#include <string>
#include "keys.hpp"

enum EVENT_TYPE {
    ENGINE_EMPTY = 1 << 0, 
    ENGINE_TEXT_EVENT = 1 << 1,
    ENGINE_KEY_EVENT = 1 << 2,
    ENGINE_MOUSE_EVENT = 1 << 3,
    ENGINE_UI_EVENT = 1 << 4,
    ENGINE_APPLICATION_EVENT = 1 << 5,
    ENGINE_WINDOW_RESIZE_EVENT = 1 << 6
};

class Event {
public:
    Event() {}
    ~Event() {}
    
    virtual EVENT_TYPE get_type() const = 0;
    //static const EVENT_TYPE type = EMPTY;

private:
};


class TextEvent: public Event {
public:
    TextEvent(std::string data):  data(data) {}
    std::string data;

    static EVENT_TYPE get_static_type() { return ENGINE_TEXT_EVENT; }
    virtual EVENT_TYPE get_type() const override { return get_static_type(); }
};

enum APP_FLAG {
    QUIT
};

class ApplicationEvent: public Event {
public:
    ApplicationEvent(APP_FLAG data):  data(data) {}
    APP_FLAG data;

    static EVENT_TYPE get_static_type() { return ENGINE_APPLICATION_EVENT; }
    virtual EVENT_TYPE get_type() const override { return get_static_type(); }
};

class WindowResize: public Event {
public:
    WindowResize(int width, int height):  width(width), height(height) {}
    int width;
    int height;

    static EVENT_TYPE get_static_type() { return ENGINE_WINDOW_RESIZE_EVENT; }
    virtual EVENT_TYPE get_type() const override { return get_static_type(); }
};

class KeyEvent: public Event {
public:
    KeyEvent(int key, int action):  key(key), action(action) {}
    const int key;
    const int action;

    static EVENT_TYPE get_static_type() { return ENGINE_KEY_EVENT; }
    virtual EVENT_TYPE get_type() const override { return get_static_type(); }
};
#endif
