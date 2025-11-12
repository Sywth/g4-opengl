/**
 * Flags
 * _LOG_LEVEL_<level> : Set the current log level (<level>). Undefined
 *  defaults to DEBUG.
 * _LOG_FLUSH : Flush the log output after each message
 */

#pragma once
#include <iostream>
#include <string_view>

enum class LogLevel { Debug, Info, Warn, Error, Fatal, None };

constexpr LogLevel CURRENT_LOG_LEVEL =
#if defined(_LOG_LEVEL_DEBUG)
    LogLevel::Debug;
#elif defined(_LOG_LEVEL_INFO)
    LogLevel::Info;
#elif defined(_LOG_LEVEL_WARN)
    LogLevel::Warn;
#elif defined(_LOG_LEVEL_ERROR)
    LogLevel::Error;
#elif defined(_LOG_LEVEL_FATAL)
    LogLevel::Fatal;
#else
    LogLevel::Debug;
#endif

template <LogLevel L>
inline void log(std::string_view msg) {
    if constexpr (L < CURRENT_LOG_LEVEL) {
        return;
    }

    switch (L) {
        case LogLevel::Debug:
            std::cout << "[DEBUG] ";
            break;
        case LogLevel::Info:
            std::cout << "[INFO] ";
            break;
        case LogLevel::Warn:
            std::cout << "[WARN] ";
            break;
        case LogLevel::Error:
            std::cout << "[ERROR] ";
            break;
        case LogLevel::Fatal:
            std::cout << "[FATAL] ";
            break;
        default:
            break;
    }

    std::cout << msg << '\n';

#if defined(_LOG_FLUSH)
    std::cout.flush();
#endif
}