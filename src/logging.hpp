#pragma once
#include <iostream>
#include <string_view>

enum class LogLevel { Debug, Info, Warn, Error, Fatal, None };

template <typename = void>
struct LogConfig {
    static constexpr LogLevel current_log_level = LogLevel::Info;
    static constexpr bool flush_log = true;
};

template <LogLevel L>
void log(std::string_view msg) {
    if constexpr (L < LogConfig<>::current_log_level) {
        return;
    }

    // This part will only be compiled for log levels >= current_log_level
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

    if constexpr (LogConfig<>::flush_log) {
        std::cout.flush();
    }
}