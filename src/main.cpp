#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "_config_debug.hpp"
#include "camera.hpp"
#include "constants.hpp"
#include "game_state.hpp"
#include "gl_debug.hpp"
#include "logger.hpp"
#include "math.hpp"
#include "mesh.hpp"
#include "render.hpp"
#include "shader.hpp"
#include "transform.hpp"

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
#include <entt/entt.hpp>

void handle_window_events(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        log<LogLevel::Info>("Escape key pressed, closing window");
        glfwSetWindowShouldClose(window, true);
    }

    g4::game_state::input_move = glm::vec2(0.0f, 0.0f);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        g4::game_state::input_move.y = 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        g4::game_state::input_move.y = -1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        g4::game_state::input_move.x = -1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        g4::game_state::input_move.x = 1.0f;
    }
}

void cursor_move_callback(GLFWwindow* window, double xpos, double ypos) {
    static bool first_mouse = true;
    static glm::vec2 last_pos = glm::vec2(0.0f, 0.0f);
    if (first_mouse) {
        last_pos = glm::vec2(static_cast<float>(xpos), static_cast<float>(-ypos));
        first_mouse = false;
        return;
    }

    glm::vec2 current_pos = glm::vec2(static_cast<float>(xpos), static_cast<float>(-ypos));

    glm::vec2 offset = current_pos - last_pos;
    last_pos = current_pos;

    g4::game_state::input_look = offset * g4::game_state::mouse_sensitivity;
}

void cursor_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    g4::game_state::fov -= static_cast<float>(yoffset);
    g4::game_state::fov = glm::clamp(g4::game_state::fov, 1.0f, 90.0f);
    log<LogLevel::Info>(std::format("FOV adjusted to {:.2f}", g4::game_state::fov));
}

const aiScene* load_scene(const std::filesystem::path& path, Assimp::Importer& importer) {
    if (!std::filesystem::exists(path)) {
        log<LogLevel::Error>("No model found at " + std::filesystem::absolute(path).string());
        return nullptr;
    }

    const aiScene* scene = importer.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)) {
        log<LogLevel::Error>("Assimp error: " + std::string(importer.GetErrorString()));
        return nullptr;
    }

    return scene;
}

// This function assumes the mesh was loaded with aiProcess_Triangulate flag
//  TODO : Refactor this
void load_trianuglated_mesh_data(const aiMesh* mesh, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
    if (!mesh) {
        log<LogLevel::Error>("Null mesh provided to load_mesh_data");
        return;
    }

    vertices.reserve(mesh->mNumVertices * 3);
    const float scale = 1.0f;  // DEBUG
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        const aiVector3D& pos = mesh->mVertices[i];

        vertices.push_back(pos.x * scale);
        vertices.push_back(pos.y * scale);
        vertices.push_back(pos.z * scale);
    }

    indices.reserve(mesh->mNumFaces * 3);
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        const aiFace& face = mesh->mFaces[i];
        assert((face.mNumIndices == 3) && "non triangulated face found while trying to load mesh data");

        indices.push_back(face.mIndices[0]);
        indices.push_back(face.mIndices[1]);
        indices.push_back(face.mIndices[2]);
    }
}

void resize_cb(GLFWwindow* window, int width, int height) {
    log<LogLevel::Info>(std::format("Window resized to {}x{}", width, height));
    g4::game_state::width = width;
    g4::game_state::height = height;
    GL_CALL(glViewport(0, 0, width, height));
};

int main() {
    GLFWwindow* window;
    if (!glfwInit()) {
        log<LogLevel::Error>("Failed to initialize GLFW");
        return -1;
    }

    // Give windowing library hints for debugging and opengl stuff (Before
    // creating window)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    if constexpr (debug_enabled) {
        log<LogLevel::Info>("DEBUG flag on");
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    } else {
        log<LogLevel::Info>("DEBUG flag off");
    }

    log<LogLevel::Info>("GLFW initialized successfully");
    window = glfwCreateWindow(g4::game_state::width, g4::game_state::height, "GLFW Window Test", nullptr, nullptr);
    if (!window) {
        log<LogLevel::Error>("Failed to create window");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, resize_cb);
    glfwSetCursorPosCallback(window, cursor_move_callback);
    glfwSetScrollCallback(window, cursor_scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        log<LogLevel::Error>("Failed to initialize GLAD");
        glfwTerminate();
        return -1;
    }

    // Enable OpenGL debug context if available (Requires 4.3+)
    if constexpr (debug_enabled) {
        int flags;
        GL_CALL(glGetIntegerv(GL_CONTEXT_FLAGS, &flags));
        if ((flags & GL_CONTEXT_FLAG_DEBUG_BIT) == 0) {
            log<LogLevel::Fatal>(
                "Debug context was not available even when DEBUG flag was "
                "set");
            return -1;
        }

        GL_CALL(glEnable(GL_DEBUG_OUTPUT));
        GL_CALL(glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS));
        GL_CALL(glDebugMessageCallback(gl_debug_output, nullptr));
        GL_CALL(glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE));
        log<LogLevel::Info>("OpenGL debug context enabled");
    }

    GL_CALL(glEnable(GL_DEPTH_TEST));
    GL_CALL(glEnable(GL_CULL_FACE));
    GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
    {
        // --- Loading Data ---
        Assimp::Importer importer;
        const aiScene* scene = load_scene("../../assets/models/cube/cube.obj", importer);
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

        // --- Shaders ---
        auto basic_shader = Shader("../../assets/shaders/basic.vert", "../../assets/shaders/basic.frag");

        // --- ECS Setup ---
        entt::registry entt_registry{};

        // Create mesh entity
        entt::entity e_mesh = entt_registry.create();
        entt_registry.emplace<c_Mesh>(e_mesh, c_Mesh{vertices, indices});
        entt_registry.emplace<c_Transform>(e_mesh, c_Transform{});

        // Create camera entity
        entt::entity e_camera = entt_registry.create();
        entt_registry.emplace<c_Transform>(e_camera, c_Transform{});
        entt_registry.emplace<c_Camera>(e_camera, c_Camera{});

        //   --- Render Loop ---
        while (!glfwWindowShouldClose(window)) {
            handle_window_events(window);

            // Delta Time
            float time_current_frame = static_cast<float>(glfwGetTime());
            g4::game_state::delta_time = time_current_frame - g4::game_state::time_last_frame;
            g4::game_state::time_last_frame = time_current_frame;

            // Rendering
            GL_CALL(glClearColor(g4::game_state::clear_color.x, g4::game_state::clear_color.y,
                                 g4::game_state::clear_color.z, 1.0f));
            GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
            basic_shader.use();

            // TODO : This new quatternion shit broke everything (check last
            // commit) Camera updates
            s_camera(entt_registry);
            // RF: There shoould be a bettter location to reset inputs
            g4::game_state::input_look = glm::vec2(0.0f, 0.0f);

            // Update zoom
            auto c_camera_transform = entt_registry.get<c_Transform>(e_camera);
            auto c_camera_camera = entt_registry.get<c_Camera>(e_camera);

            // RF: This is ugly
            // Updates aspect ratio
            c_camera_camera.aspect_ratio =
                static_cast<float>(g4::game_state::width) / static_cast<float>(g4::game_state::height);

            glm::mat4 mat_model = glm::mat4(1.0f);
            glm::mat4 mat_view = c_camera_transform.view_matrix();
            glm::mat4 mat_proj = glm::perspective(glm::radians(c_camera_camera.fov_deg), c_camera_camera.aspect_ratio,
                                                  c_camera_camera.z_near, c_camera_camera.z_far);

            basic_shader.set_mat4f("uModel", mat_model);
            basic_shader.set_mat4f("uView", mat_view);
            basic_shader.set_mat4f("uProj", mat_proj);
            g4::gapi::s_gl_render(entt_registry);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        g4::gapi::get_gl_graphics_api_instance().cleanup_all();
    }

    glfwTerminate();
    return 0;
}