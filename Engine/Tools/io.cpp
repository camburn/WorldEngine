#include "engine.hpp"
#include "io.hpp"


std::string read_file(std::string& file_path) {
    ENGINE_DEBUG("Opening {0}", file_path);

    std::ifstream file_stream(file_path);
    if (!file_stream) {
        ENGINE_ERROR("File {0} cannot be opened", file_path);
    }
    std::ostringstream buffer;
    buffer << file_stream.rdbuf();
    std::string data = buffer.str();
    return data;
}
