// Note : assume that opengl debugging context has be setup elsewhere
#pragma once
#include "_config_debug.hpp"
#include "logger.hpp"

#include <glad/glad.h>

void APIENTRY gl_debug_output(GLenum source,
                              GLenum type,
                              unsigned int id,
                              GLenum severity,
                              GLsizei length,
                              const char* message,
                              const void* userParam);

const char* translate_gl_get_error_enum_message(GLenum error_code);

// TODO : Translate the OpenGL error code to human-readable string
inline void check_gl_error(const char* file, int line) {
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cout << "[GL CALL ERR] at " << file << ":" << line
                  << " - Error code: 0x" << std::hex << error << std::dec
                  << "\n\t(" << translate_gl_get_error_enum_message(error)
                  << ")" << std::endl;
    }
}

#define GL_CALL(x)                              \
    do {                                        \
        if constexpr (debug_enabled) {          \
            x;                                  \
            check_gl_error(__FILE__, __LINE__); \
        } else {                                \
            x;                                  \
        }                                       \
    } while (0)