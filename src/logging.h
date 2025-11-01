#pragma once

#include <iostream>

#if !defined(_LOG_LEVEL_DEBUG) && !defined(_LOG_LEVEL_INFO) && !defined(_LOG_LEVEL_WARN) && !defined(_LOG_LEVEL_ERROR)
#define _LOG_LEVEL_DEBUG
#endif

#if defined(_LOG_LEVEL_DEBUG)
    #define LOG_DEBUG(msg) (std::cout << "[DEBUG] " << (msg) << std::endl)
    #define LOG_INFO(msg)  (std::cout << "[INFO] "  << (msg) << std::endl)
    #define LOG_WARN(msg)  (std::cout << "[WARN] "  << (msg) << std::endl)
    #define LOG_ERROR(msg) (std::cerr << "[ERROR] " << (msg) << std::endl)
#elif defined(_LOG_LEVEL_INFO)
    #define LOG_DEBUG(msg) ((void)0)
    #define LOG_INFO(msg)  (std::cout << "[INFO] "  << (msg) << std::endl)
    #define LOG_WARN(msg)  (std::cout << "[WARN] "  << (msg) << std::endl)
    #define LOG_ERROR(msg) (std::cerr << "[ERROR] " << (msg) << std::endl)
#elif defined(_LOG_LEVEL_WARN)
    #define LOG_DEBUG(msg) ((void)0)
    #define LOG_INFO(msg)  ((void)0)
    #define LOG_WARN(msg)  (std::cout << "[WARN] "  << (msg) << std::endl)
    #define LOG_ERROR(msg) (std::cerr << "[ERROR] " << (msg) << std::endl)
#elif defined(_LOG_LEVEL_ERROR)
    #define LOG_DEBUG(msg) ((void)0)
    #define LOG_INFO(msg)  ((void)0)
    #define LOG_WARN(msg)  ((void)0)
    #define LOG_ERROR(msg) (std::cerr << "[ERROR] " << (msg) << std::endl)
#else
    #define LOG_DEBUG(msg) (std::cout << "[DEBUG] " << (msg) << std::endl)
    #define LOG_INFO(msg)  (std::cout << "[INFO] "  << (msg) << std::endl)
    #define LOG_WARN(msg)  (std::cout << "[WARN] "  << (msg) << std::endl)
    #define LOG_ERROR(msg) (std::cerr << "[ERROR] " << (msg) << std::endl)
#endif