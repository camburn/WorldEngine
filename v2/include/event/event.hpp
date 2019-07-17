#ifndef _EVENT_HPP
#define _EVENT_HPP
#include <string>

enum EVENT_TYPE {
    EMPTY = 1,
    TEXT_EVENT = 2,
    KEY_EVENT = 4,
    MOUSE_EVENT = 8,
    UI_EVENT = 16,
    APPLICATION_EVENT = 32
};


class Event {
public:
    Event() {}
    ~Event() {}
    virtual EVENT_TYPE get_type() const = 0;
    static const EVENT_TYPE type = EMPTY;

private:
};


class TextEvent: public Event {
public:
    TextEvent(std::string data):  data(data) {}
    std::string data;

    virtual EVENT_TYPE get_type() const override { return type; }
    static const EVENT_TYPE type = TEXT_EVENT;
};

enum APP_FLAG {
    QUIT
};

class ApplicationEvent: public Event {
public:
    ApplicationEvent(APP_FLAG data):  data(data) {}
    APP_FLAG data;

    virtual EVENT_TYPE get_type() const override { return type; }
    static const EVENT_TYPE type = APPLICATION_EVENT;
};
#endif
