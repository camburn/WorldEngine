#include "identifier.hpp"

long current_id = 10000;

long get_uid() {
    current_id++;
    return current_id;
}
