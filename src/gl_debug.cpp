// From https://learnopengl.com/In-Practice/Debugging
//  Credit to Joey de Vries
#include "gl_debug.hpp"
#include <iostream>

void APIENTRY gl_debug_output(GLenum source,
                              GLenum type,
                              unsigned int id,
                              GLenum severity,
                              GLsizei length,
                              const char* message,
                              const void* userParam) {
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source) {
        case GL_DEBUG_SOURCE_API:
            std::cout << "Source: API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            std::cout << "Source: Window System";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            std::cout << "Source: Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            std::cout << "Source: Third Party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            std::cout << "Source: Application";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            std::cout << "Source: Other";
            break;
    }
    std::cout << std::endl;

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            std::cout << "Type: Error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            std::cout << "Type: Deprecated Behaviour";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            std::cout << "Type: Undefined Behaviour";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            std::cout << "Type: Portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            std::cout << "Type: Performance";
            break;
        case GL_DEBUG_TYPE_MARKER:
            std::cout << "Type: Marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            std::cout << "Type: Push Group";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            std::cout << "Type: Pop Group";
            break;
        case GL_DEBUG_TYPE_OTHER:
            std::cout << "Type: Other";
            break;
    }
    std::cout << std::endl;

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            std::cout << "Severity: high";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            std::cout << "Severity: medium";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            std::cout << "Severity: low";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            std::cout << "Severity: notification";
            break;
    }
    std::cout << std::endl;
    std::cout << std::endl;
}

// From https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetError.xhtml
const char* translate_gl_get_error_enum_message(GLenum error_code) {
    switch (error_code) {
        case GL_NO_ERROR:
            return "No error has been recorded. The value of this symbolic "
                   "constant is guaranteed to be 0.";
        case GL_INVALID_ENUM:
            return "An unacceptable value is specified for an enumerated "
                   "argument. The offending command is ignored and has no "
                   "other side effect than to set the error flag.";
        case GL_INVALID_VALUE:
            return "A numeric argument is out of range. The offending command "
                   "is ignored and has no other side effect than to set the "
                   "error flag.";
        case GL_INVALID_OPERATION:
            return "The specified operation is not allowed in the current "
                   "state. The offending command is ignored and has no other "
                   "side effect than to set the error flag.";
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "The framebuffer object is not complete. The offending "
                   "command is ignored and has no other side effect than to "
                   "set the error flag.";
        case GL_OUT_OF_MEMORY:
            return "There is not enough memory left to execute the command. "
                   "The state of the GL is undefined, except for the state of "
                   "the error flags, after this error is recorded.";
        case GL_STACK_UNDERFLOW:
            return "An attempt has been made to perform an operation that "
                   "would cause an internal stack to underflow.";
        case GL_STACK_OVERFLOW:
            return "An attempt has been made to perform an operation that "
                   "would cause an internal stack to overflow.";
        default:
            return "Unknown error code";
    }
}