#pragma once
#include "logger_config.hpp"

#include <iostream>
#include <string_view>

template <LogLevel L>
void log(std::string_view msg) {
    if constexpr (L < current_log_level) {
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

    if constexpr (flush_log) {
        std::cout.flush();
    }
}