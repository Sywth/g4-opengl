#pragma once
#include <glad/glad.h>
#include "logging.h"

void APIENTRY gl_debug_output(GLenum source,
                              GLenum type,
                              unsigned int id,
                              GLenum severity,
                              GLsizei length,
                              const char* message,
                              const void* userParam);

// TODO : Translate the OpenGL error code to human-readable string
inline void check_gl_error(const char* file, int line) {
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cerr << "[GL CALL ERR] at " << file << ":" << line
                  << " - Error code: 0x" << std::hex << error << std::dec
                  << std::endl;
    }
}

// Macro to wrap OpenGL calls with error checking
#ifdef DEBUG
#define GL_CALL(x)                          \
    do {                                    \
        x;                                  \
        check_gl_error(__FILE__, __LINE__); \
    } while (0)
#else
#define GL_CALL(x) x
#endif