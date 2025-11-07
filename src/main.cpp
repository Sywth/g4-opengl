#define _LOG_LEVEL_DEBUG
#define DEBUG

#include "debug.h"
#include "shader.h"
#include "triangle_mesh.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>

// Glad must be included before GLFW
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

// float vertices[] = {
//     0.5f,  -0.5f, 0.0f,  // Bottom Right
//     0.5f,  0.5f,  0.0f,  // Top Right
//     -0.5f, 0.5f,  0.0f,  // Top Left
//     -0.5f, -0.5f, 0.0f,  // Bottom Left
// };
// unsigned int indices[] = {
//     0, 1, 3,  // First Triangle
//     1, 2, 3   // Second Triangle
// };

void handle_window_events(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        LOG_INFO("Escape key pressed, closing window");
        glfwSetWindowShouldClose(window, true);
    }
}

bool load_scene(const std::filesystem::path& path,
                const aiScene** outScene,
                Assimp::Importer& importer) {
    if (!std::filesystem::exists(path)) {
        LOG_ERROR("No model found at " +
                  std::filesystem::absolute(path).string());
        return false;
    }

    *outScene = importer.ReadFile(
        path.string(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices |
                           aiProcess_SortByPType);

    if (!(*outScene) || (*outScene)->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !(*outScene)->mRootNode) {
        LOG_ERROR("Assimp error: " + std::string(importer.GetErrorString()));
        return false;
    }

    return true;
}

std::tuple<std::vector<float>, std::vector<unsigned int>> load_mesh_data(
    const aiScene* scene,
    unsigned int meshIndex) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    if (meshIndex >= scene->mNumMeshes) {
        LOG_ERROR("Mesh index out of bounds");
        return {vertices, indices};
    }

    const aiMesh* mesh = scene->mMeshes[meshIndex];
    if (!mesh) {
        LOG_ERROR("Failed to retrieve mesh");
        return {vertices, indices};
    }

    // Process vertices
    vertices.reserve(mesh->mNumVertices * 3);
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        const aiVector3D& pos = mesh->mVertices[i];
        vertices.push_back(pos.x);
        vertices.push_back(pos.y);
        vertices.push_back(pos.z);
    }

    // Process indices
    indices.reserve(mesh->mNumFaces * 3);
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        const aiFace& face = mesh->mFaces[i];
        indices.insert(indices.end(), face.mIndices,
                       face.mIndices + face.mNumIndices);
    }

    return {vertices, indices};
}

int main() {
    // TODO : Get this mesh loading into my custom triangle mesh class working
    //  - I think it is working its just
    //      - 1) It wrong ordering (must be ccw)
    //      - 2) Its too big
    Assimp::Importer importer;
    const aiScene* scene = nullptr;
    if (!load_scene("../../assets/models/cube/cube.obj", &scene, importer)) {
        return -1;
    }
    auto [vertices, indices] = load_mesh_data(scene, 0);

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

    TriangleMesh triangle_mesh(
        std::vector<float>(std::begin(vertices), std::end(vertices)),
        std::vector<unsigned int>(std::begin(indices), std::end(indices)),
        shaderProgram);

    GL_CALL(glEnable(GL_CULL_FACE));
    while (!glfwWindowShouldClose(window)) {
        handle_window_events(window);

        GL_CALL(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT));

        triangle_mesh.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}