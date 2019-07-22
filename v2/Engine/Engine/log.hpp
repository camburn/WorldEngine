#ifndef _LOG_HPP
#define _LOG_HPP
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace engine {

class Log
{
public:
    static void Init();

    inline static std::shared_ptr<spdlog::logger>& GetEngineLogger() { return s_EngineLogger; }
    inline static std::shared_ptr<spdlog::logger>& GetGameLogger() { return s_GameLogger; }
private:
    static std::shared_ptr<spdlog::logger> s_EngineLogger;
    static std::shared_ptr<spdlog::logger> s_GameLogger;
};

}

#ifdef _MSC_VER
#define DEBUG_BREAK __debugbreak()
#else
#include <csignal>
#define DEBUG_BREAK raise(SIGTRAP);
#endif

// Engine log macros
#define ENGINE_TRACE(...)    ::engine::Log::GetEngineLogger()->trace(__VA_ARGS__)
#define ENGINE_INFO(...)     ::engine::Log::GetEngineLogger()->info(__VA_ARGS__)
#define ENGINE_WARN(...)     ::engine::Log::GetEngineLogger()->warn(__VA_ARGS__)
#define ENGINE_ERROR(...)    ::engine::Log::GetEngineLogger()->error(__VA_ARGS__)
#define ENGINE_FATAL(...)    ::engine::Log::GetEngineLogger()->fatal(__VA_ARGS__)

// Game log macros
#define GAME_TRACE(...)     ::engine::Log::GetGameLogger()->trace(__VA_ARGS__)
#define GAME_INFO(...)      ::engine::Log::GetGameLogger()->info(__VA_ARGS__)
#define GAME_WARN(...)      ::engine::Log::GetGameLogger()->warn(__VA_ARGS__)
#define GAME_ERROR(...)     ::engine::Log::GetGameLogger()->error(__VA_ARGS__)
#define GAME_FATAL(...)     ::engine::Log::GetGameLogger()->fatal(__VA_ARGS__)

#define GAME_ASSERT(x, ...) { if(!(x)) { GAME_ERROR("Assertion Failed: {0}", __VA_ARGS__); DEBUG_BREAK; } }
#define ENGINE_ASSERT(x, ...) { if(!(x)) { ENGINE_ERROR("Assertion Failed: {0}", __VA_ARGS__); DEBUG_BREAK; } }

#endif
