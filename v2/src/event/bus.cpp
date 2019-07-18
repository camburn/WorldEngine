#include "engine.hpp"
#include "bus.hpp"


namespace bus {

class SubBuffer {
public:
    SubBuffer(){};
    SubBuffer(std::string name): name(name) {}
    SubBuffer(std::string name, int filter): name(name), filter_value(filter) {}

    void add(std::shared_ptr<Event> event) {
        events.push(std::move(event));
    }

    std::shared_ptr<Event> get() {
        if (!events.empty()) {
            std::shared_ptr<Event> event = std::move(events.front());
            events.pop();
            return event;
        }
        return nullptr;
    }

    bool filter(EVENT_TYPE type) {
        if (filter_value == -1) return true;
        return type & filter_value;
     }

private:
    std::string name;
    int filter_value = -1;
    std::queue<std::shared_ptr<Event>> events;
};

std::map<std::string, SubBuffer> subscriptions;
std::vector<callback> callbacks;

void publish(std::unique_ptr<Event> pub_msg) {
    // Publish a event to all subscriptions and callbacks that match their filters
    std::shared_ptr<Event> event = std::move(pub_msg);
    // NOTE: c++17 feature https://en.cppreference.com/w/cpp/language/structured_binding
    for ([[maybe_unused]]auto &[name, buffer]: subscriptions) {
        (void)name;  // Name unused
        if (buffer.filter(event->get_type()))
            buffer.add(event);
    }
    for (auto &[func, filter]: callbacks) {
        if (filter & event->get_type()) {
            func(event);
        }
    }
}

void register_callback(callback_prototype func, int filter) {
    // Recieve callback events when a event appears that matches the filter
    callbacks.push_back(callback(func, filter));
}

void subscribe(std::string name) {
    // Create a subscription to all events, subcriptions can be checked with bus::get
    subscribe(name, -1);
}

void subscribe(std::string name, int filter) {
    // Create a filtered subscription, subcriptions can be checked with bus::get
    ENGINE_INFO("Subscribed to event queue: '{0}'", name);
    subscriptions[name] = SubBuffer{name, filter};
}

std::shared_ptr<Event> get(std::string name) {
    // Get a event from a named subscription return subscriptions[name].get();
    return subscriptions[name].get();
}

}
