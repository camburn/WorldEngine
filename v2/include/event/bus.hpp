#ifndef _BUS_HPP
#define _BUS_HPP
#include <string>
#include <map>
#include <vector>
#include <queue>
#include <functional>
#include <memory>

#include "event.hpp"

namespace bus {

typedef std::function<void(std::shared_ptr<Event>)> callback_prototype;
typedef std::pair<callback_prototype, int> callback;

void publish(std::unique_ptr<Event> event);
void register_callback(callback_prototype func, int filter);
void subscribe(std::string name);
void subscribe(std::string name, int filter);
std::shared_ptr<Event> get(std::string name);

template<class T>
std::shared_ptr<T> get_event(std::shared_ptr<Event> &event) {
    if (event != nullptr && event->get_type() == T::type) {
        return std::dynamic_pointer_cast<T>(event);
    }
    return nullptr;
}

}  //namespace

#endif
