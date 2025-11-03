#define _LOG_LEVEL_DEBUG
#define DEBUG

#include "debug.h"
#include "shader.h"

// Glad must be included before GLFW
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <filesystem>
#include <fstream>
#include <iostream>

// TODO :
// - Figure out why the triangle is not being rendered
// FIXME : Does not render the triangle :(

float vertices[] = {
    -0.5f, -0.5f, 0.0f,  // left
    0.5f,  -0.5f, 0.0f,  // right
    0.0f,  0.5f,  0.0f   // top
};
unsigned int indices[] = {2, 1, 0};

void handle_window_events(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        LOG_INFO("Escape key pressed, closing window");
        glfwSetWindowShouldClose(window, true);
    }
}

int main() {
    GLFWwindow* window;

    if (!glfwInit()) {
        LOG_ERROR("Failed to initialize GLFW");
        return -1;
    }

    // Give windowing library hints for debugging and opengl stuff (Before
    // creating window)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

#ifdef DEBUG
    LOG_INFO("DEBUG flag on");
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#else
    LOG_INFO("DEBUG flag off");
#endif

    LOG_INFO("GLFW initialized successfully");
    window = glfwCreateWindow(640, 480, "GLFW Window Test", nullptr, nullptr);
    if (!window) {
        LOG_ERROR("Failed to create window");
        glfwTerminate();
        return -1;
    }

    auto resize_cb = [](GLFWwindow* window, int width, int height) {
        LOG_INFO(std::format("Window resized to {}x{}", width, height));
        GL_CALL(glViewport(0, 0, width, height));
    };

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, resize_cb);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        LOG_ERROR("Failed to initialize GLAD");
        glfwTerminate();
        return -1;
    }

// Enable OpenGL debug context if available (Requires 4.3+)
#ifdef DEBUG
    int flags;
    GL_CALL(glGetIntegerv(GL_CONTEXT_FLAGS, &flags));
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        GL_CALL(glEnable(GL_DEBUG_OUTPUT));
        GL_CALL(glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS));
        GL_CALL(glDebugMessageCallback(gl_debug_output, nullptr));
        GL_CALL(glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE,
                                      0, nullptr, GL_TRUE));
        LOG_INFO("OpenGL debug context enabled");
    } else {
        throw std::runtime_error(
            "Debug context was not available even when DEBUG flag was set");
    }
#endif

    auto shaderProgram = create_shader_program("../../shaders/basic.vert",
                                               "../../shaders/basic.frag");

    unsigned int vao, vbo, ebo;
    GL_CALL(glGenVertexArrays(1, &vao));
    GL_CALL(glGenBuffers(1, &vbo));
    GL_CALL(glGenBuffers(1, &ebo));

    GL_CALL(glBindVertexArray(vao));

    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                         GL_STATIC_DRAW));

    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                         GL_STATIC_DRAW));

    GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                                  (void*)0));
    GL_CALL(glEnableVertexAttribArray(0));

    // Unbind for sanity
    GL_CALL(glBindVertexArray(0));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

    while (!glfwWindowShouldClose(window)) {
        handle_window_events(window);

        GL_CALL(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT));

        GL_CALL(glUseProgram(shaderProgram));
        GL_CALL(glBindVertexArray(vao));
        GL_CALL(glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    GL_CALL(glDeleteProgram(shaderProgram));
    GL_CALL(glDeleteVertexArrays(1, &vao));
    GL_CALL(glDeleteBuffers(1, &vbo));
    GL_CALL(glDeleteBuffers(1, &ebo));
    glfwTerminate();
    return 0;
}