#define _LOG_LEVEL_DEBUG
#define DEBUG

#include "debug.h"
#include "shader.h"
#include "triangle_mesh.h"

// Glad must be included before GLFW
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <filesystem>
#include <fstream>
#include <iostream>

std::vector<float> vertices = {
    -0.5f, -0.5f, 0.0f,  // bottom left
    0.5f,  -0.5f, 0.0f,  // bottom right
    0.0f,  0.5f,  0.0f   // top
};
std::vector<unsigned int> indices = {0, 1, 2};

// TODO :
// - Figure out why the triangle is not being rendered
// FIXME : Does not render the triangle :(

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

  glfwMakeContextCurrent(window);

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
    GL_CALL(glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0,
                                  nullptr, GL_TRUE));
    LOG_INFO("OpenGL debug context enabled");
  } else {
    throw std::runtime_error(
        "Debug context was not available even when DEBUG flag was set");
  }
#endif

  auto shaderProgram = create_shader_program("../../shaders/basic.vert",
                                             "../../shaders/basic.frag");
  TriangleMesh mesh(vertices, indices, shaderProgram);

  GL_CALL(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
    mesh.Draw();

    glfwSwapBuffers(window);
  }

  GL_CALL(glDeleteProgram(shaderProgram));
  glfwTerminate();
  return 0;
}