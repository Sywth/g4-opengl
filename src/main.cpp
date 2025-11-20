#include "_config_debug.hpp"
#include "camera.hpp"
#include "constants.hpp"
#include "game_state.hpp"
#include "gl_debug.hpp"
#include "logger.hpp"
#include "math.hpp"
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

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
        last_pos =
            glm::vec2(static_cast<float>(xpos), static_cast<float>(-ypos));
        first_mouse = false;
        return;
    }

    glm::vec2 current_pos =
        glm::vec2(static_cast<float>(xpos), static_cast<float>(-ypos));

    glm::vec2 offset = current_pos - last_pos;
    last_pos = current_pos;

    g4::game_state::input_look = offset * g4::game_state::mouse_sensitivity;
}

void cursor_scroll_callback(GLFWwindow* window,
                            double xoffset,
                            double yoffset) {
    g4::game_state::fov -= static_cast<float>(yoffset);
    g4::game_state::fov = glm::clamp(g4::game_state::fov, 1.0f, 90.0f);
    log<LogLevel::Info>(
        std::format("FOV adjusted to {:.2f}", g4::game_state::fov));
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
        assert((face.mNumIndices == 3) &&
               "non triangulated face found while trying to load mesh data");

        indices.push_back(face.mIndices[0]);
        indices.push_back(face.mIndices[1]);
        indices.push_back(face.mIndices[2]);
    }
}

struct c_Transform {
    glm::vec3 position{0.0f};
    glm::vec3 scale{1.0f};
    glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
};

struct c_Camera {
    float fov_deg{64.0f};
    float z_near{0.0625f};
    float z_far{256.0f};
    float aspect_ratio{1.0f};
};

// TODO : Get this mesh loading into my custom triangle mesh class working
//  - I think it is working its just
//      - 1) It wrong ordering (must be ccw)
//      - 2) Its too big
int main() {
    GLFWwindow* window;
    if (!glfwInit()) {
        log<LogLevel::Error>("Failed to initialize GLFW");
        return -1;
    }

    // TODO : Finish using this
    entt::registry entt_registry{};
    entt::entity player = entt_registry.create();
    entt_registry.emplace<c_Transform>(player, glm::vec3(1.0f), glm::vec3(2.0f),
                                       glm::vec3(3.0f));

    auto player_transform = entt_registry.get<c_Transform>(player);
    auto position = player_transform.position;
    auto scale = player_transform.scale;
    auto rotation = glm::eulerAngles(player_transform.rotation);
    log<LogLevel::Info>(std::format(
        "Player Transform : \nPos : ({:.2f}, {:.2f}, {:.2f})\nScale : ({:.2f}, "
        "{:.2f}, {:.2f})\nRotation : ({:.2f}, {:.2f}, {:.2f})",
        position.x, position.y, position.z, scale.x, scale.y, scale.z,
        rotation.x, rotation.y, rotation.z));

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
                "Debug context was not available even when DEBUG flag was set");
            return -1;
        }

        GL_CALL(glEnable(GL_DEBUG_OUTPUT));
        GL_CALL(glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS));
        GL_CALL(glDebugMessageCallback(gl_debug_output, nullptr));
        GL_CALL(glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE,
                                      0, nullptr, GL_TRUE));
        log<LogLevel::Info>("OpenGL debug context enabled");
    }

    GL_CALL(glEnable(GL_DEPTH_TEST));
    GL_CALL(glEnable(GL_CULL_FACE));
    GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
    {
        // --- Loading Data ---
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

        TriangleMesh triangle_mesh(vertices, indices);

        // --- Shaders ---
        auto basic_shader = Shader("../../assets/shaders/basic.vert",
                                   "../../assets/shaders/basic.frag");

        // --- Camera ---
        glm::mat4 mat_model = glm::mat4(1.0f);  // local -> world
        glm::mat4 mat_view = glm::mat4(1.0f);   // world -> camera
        glm::mat4 mat_proj = glm::perspective(
            glm::radians(g4::game_state::fov),
            g4::config::display::aspect_ratio, g4::config::display::z_near,
            g4::config::display::z_far);  // camera -> clip (screen)

        float initial_z = -8.0f;
        mat_view = glm::translate(mat_view, glm::vec3(0.0f, 0.0f, initial_z));

        glm::vec3 offsets[] = {
            glm::vec3(-0.82, 0.34, 0.12f), glm::vec3(0.45f, -0.12f, -0.34f),
            glm::vec3(0.12f, 0.56f, 0.78f), glm::vec3(-0.34f, -0.78f, 0.45f)};

        Camera camera(glm::vec3(0.0f, 0.0f, initial_z),
                      glm::vec3(0.0f, 0.0f, 0.0f));

        //   --- Render Loop ---
        while (!glfwWindowShouldClose(window)) {
            handle_window_events(window);

            // Delta Time
            float time_current_frame = static_cast<float>(glfwGetTime());
            g4::game_state::delta_time =
                time_current_frame - g4::game_state::time_last_frame;
            g4::game_state::time_last_frame = time_current_frame;

            // Rendering
            GL_CALL(glClearColor(g4::game_state::clear_color.x,
                                 g4::game_state::clear_color.y,
                                 g4::game_state::clear_color.z, 1.0f));
            GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
            basic_shader.use();

            // Camera updates
            camera.move_from_input(g4::game_state::input_move,
                                   g4::game_state::speed_move);
            camera.rotate_from_input(g4::game_state::input_look,
                                     g4::game_state::speed_look);
            g4::game_state::input_look = glm::vec2(0.0f, 0.0f);

            // Update zoom
            mat_proj = glm::perspective(glm::radians(g4::game_state::fov),
                                        g4::config::display::aspect_ratio,
                                        g4::config::display::z_near,
                                        g4::config::display::z_far);

            basic_shader.set_mat4f("uModel", mat_model);
            basic_shader.set_mat4f("uView", camera.get_view_matrix());
            basic_shader.set_mat4f("uProj", mat_proj);

            triangle_mesh.draw();

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    glfwTerminate();
    return 0;
}