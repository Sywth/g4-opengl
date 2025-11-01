#include "config.h"
#include <expected>

int main() {
    GLFWwindow* window;

    if (!glfwInit()) {
        std::cout<<"Failed to initialize GLFW" << std::endl;
        return -1;
    }

    std::cout << "GLFW initialized successfully" << std::endl;
    window = glfwCreateWindow(640, 480, "GLFW Window Test", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout<<"Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    glClearColor(0.8f, 0.1f, 0.8f, 1.0f);
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}