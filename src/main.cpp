#define _LOG_FLUSH
#define _LOG_LEVEL_DEBUG
#define _ENABLE_GL_CALL_DEBUG

#include "camera.hpp"
#include "config.hpp"
#include "debug.hpp"
#include "logging.hpp"
#include "shader.hpp"
#include "triangle_mesh.hpp"

#include <cassert>
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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void handle_window_events(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        log<LogLevel::Info>("Escape key pressed, closing window");
        glfwSetWindowShouldClose(window, true);
    }
}

const aiScene* load_scene(const std::filesystem::path& path,
                          Assimp::Importer& importer) {
    if (!std::filesystem::exists(path)) {
        log<LogLevel::Error>("No model found at " +
                             std::filesystem::absolute(path).string());
        return nullptr;
    }

    const aiScene* scene = importer.ReadFile(
        path.string(), aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)) {
        log<LogLevel::Error>("Assimp error: " +
                             std::string(importer.GetErrorString()));
        return nullptr;
    }

    return scene;
}

// This function assumes the mesh was loaded with aiProcess_Triangulate flag
//  TODO : Refactor this
void load_trianuglated_mesh_data(const aiMesh* mesh,
                                 std::vector<float>& vertices,
                                 std::vector<unsigned int>& indices) {
    if (!mesh) {
        log<LogLevel::Error>("Null mesh provided to load_mesh_data");
        return;
    }

    vertices.reserve(mesh->mNumVertices * 3);
    const float scale = 0.5f;  // DEBUG
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        const aiVector3D& pos = mesh->mVertices[i];

        vertices.push_back(pos.x * scale);
        vertices.push_back(pos.y * scale);
        vertices.push_back(pos.z * scale);
    }

    indices.reserve(mesh->mNumFaces * 3);
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        const aiFace& face = mesh->mFaces[i];
        assert((face.mNumIndices == 3) &&
               "non triangulated face found while trying to load mesh data");

        indices.push_back(face.mIndices[0]);
        indices.push_back(face.mIndices[1]);
        indices.push_back(face.mIndices[2]);
    }
}

// TODO : Get this mesh loading into my custom triangle mesh class working
//  - I think it is working its just
//      - 1) It wrong ordering (must be ccw)
//      - 2) Its too big
int main() {
    Assimp::Importer importer;
    const aiScene* scene =
        load_scene("../../assets/models/cube/cube.obj", importer);
    if (!scene) {
        return -1;
    }

    aiMesh* mesh = scene->mMeshes[0];
    if (!mesh) {
        return -1;
    }

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    load_trianuglated_mesh_data(mesh, vertices, indices);

    GLFWwindow* window;
    if (!glfwInit()) {
        log<LogLevel::Error>("Failed to initialize GLFW");
        return -1;
    }

    // Give windowing library hints for debugging and opengl stuff (Before
    // creating window)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

#ifdef _ENABLE_GL_CALL_DEBUG
    log<LogLevel::Info>("DEBUG flag on");
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#else
    log<LogLevel::Info>("DEBUG flag off");
#endif

    log<LogLevel::Info>("GLFW initialized successfully");
    window = glfwCreateWindow(g4::config::display::width,
                              g4::config::display::height, "GLFW Window Test",
                              nullptr, nullptr);
    if (!window) {
        log<LogLevel::Error>("Failed to create window");
        glfwTerminate();
        return -1;
    }

    auto resize_cb = [](GLFWwindow* window, int width, int height) {
        log<LogLevel::Info>(
            std::format("Window resized to {}x{}", width, height));
        GL_CALL(glViewport(0, 0, width, height));
    };

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, resize_cb);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        log<LogLevel::Error>("Failed to initialize GLAD");
        glfwTerminate();
        return -1;
    }

// Enable OpenGL debug context if available (Requires 4.3+)
#ifdef _ENABLE_GL_CALL_DEBUG
    int flags;
    GL_CALL(glGetIntegerv(GL_CONTEXT_FLAGS, &flags));
    if ((flags & GL_CONTEXT_FLAG_DEBUG_BIT) == 0) {
        log<LogLevel::Fatal>(
            "Debug context was not available even when DEBUG flag was set");
        return -1;
    }

    GL_CALL(glEnable(GL_DEBUG_OUTPUT));
    GL_CALL(glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS));
    GL_CALL(glDebugMessageCallback(gl_debug_output, nullptr));
    GL_CALL(glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0,
                                  nullptr, GL_TRUE));
    log<LogLevel::Info>("OpenGL debug context enabled");
#endif

    auto basic_shader = Shader("../../assets/shaders/basic.vert",
                               "../../assets/shaders/basic.frag");
    TriangleMesh triangle_mesh(vertices, indices);

    glm::mat4 mat_model = glm::mat4(1.0f);  // local -> world
    glm::mat4 mat_view = glm::mat4(1.0f);   // world -> camera
    glm::mat4 mat_proj = glm::perspective(
        glm::radians(45.0f), g4::config::display::aspect_ratio,
        g4::config::display::z_near,
        g4::config::display::z_far);  // camera -> clip (screen)

    float initial_z = -8.0f;
    mat_view = glm::translate(mat_view, glm::vec3(0.0f, 0.0f, initial_z));

    GL_CALL(glEnable(GL_DEPTH_TEST));
    GL_CALL(glEnable(GL_CULL_FACE));
    GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));

    glm::vec3 offsets[] = {
        glm::vec3(-1.0f, -1.0f, 0.0f),
        glm::vec3(1.0f, -1.0f, 0.0f),
        glm::vec3(-1.0f, 1.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 0.0f),
    };

    Camera camera(glm::vec3(0.0f, 0.0f, initial_z),
                  glm::vec3(0.0f, 0.0f, 0.0f));

    while (!glfwWindowShouldClose(window)) {
        handle_window_events(window);

        GL_CALL(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        basic_shader.use();

        // DEBUG : animate color
        float t = glfwGetTime();
        basic_shader.set_vecnf(
            "uColor",
            std::vector<float>{(std::sin(t) + 1.0f) / 2.0f,
                               (std::cos(t) + 1.0f) / 2.0f, 0.5f, 1.0f});

        for (const auto& offset : offsets) {
            // DEBUG : animate model matrix
            mat_model =
                glm::rotate(mat_model, glm::radians(static_cast<float>(1e-1)),
                            glm::vec3(1.0f, 1.0f, 1.0f));

            basic_shader.set_mat4f("uModel", glm::translate(mat_model, offset));
            basic_shader.set_mat4f("uView", mat_view);
            basic_shader.set_mat4f("uProj", mat_proj);

            triangle_mesh.draw();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}