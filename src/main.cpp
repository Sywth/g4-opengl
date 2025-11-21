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

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

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

    glm::vec3 forward() const {
        return glm::rotate(rotation, glm::vec3(0.0f, 0.0f, -1.0f));
    }

    glm::vec3 right() const {
        return glm::rotate(rotation, glm::vec3(1.0f, 0.0f, 0.0f));
    }

    glm::vec3 up() const {
        return glm::rotate(rotation, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    glm::mat4 view_matrix() const {
        // glm::mat4 mat_tran = glm::translate(glm::mat4(1.0f), position);
        // glm::mat4 mat_rot = glm::toMat4(rotation);
        // glm::mat4 mat_scale = glm::scale(glm::mat4(1.0f), scale);

        // return mat_tran * mat_rot * mat_scale;
        return glm::lookAt(position, position + forward(), vec3_up_world);
    }
};

struct c_Camera {
    float fov_deg{64.0f};
    float z_near{0.0625f};
    float z_far{256.0f};
    float aspect_ratio{1.0f};
};

void camera_move_system(entt::registry& registry,
                        const glm::vec2 input_move,
                        const glm::vec2 speed_move) {
    auto view = registry.view<c_Transform, c_Camera>();
    for (auto entity : view) {
        c_Transform& transform = view.get<c_Transform>(entity);

        glm::vec3 forward = transform.forward();
        glm::vec3 right = transform.right();

        transform.position += forward * input_move.y * speed_move.y;
        transform.position += right * input_move.x * speed_move.x;
    }
}

void camera_look_system(entt::registry& registry,
                        const glm::vec2 input_look,
                        const glm::vec2 speed_look) {
    auto view = registry.view<c_Transform, c_Camera>();
    for (auto entity : view) {
        c_Transform& transform = view.get<c_Transform>(entity);

        glm::quat quat_pitch = glm::angleAxis(
            glm::radians(-input_look.y * speed_look.y), transform.right());
        glm::quat quat_yaw =
            glm::angleAxis(glm::radians(-input_look.x * speed_look.x),
                           glm::vec3(0.0f, 1.0f, 0.0f));

        transform.rotation =
            glm::normalize(quat_yaw * quat_pitch * transform.rotation);
    }
}

entt::entity get_global_camera(const entt::registry& registry) {
    // TODO : For now just return the first
    auto view = registry.view<c_Transform, c_Camera>();
    for (auto entity : view) {
        return entity;
    }
    return entt::null;
}

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
    entt::entity e_camera = entt_registry.create();
    entt_registry.emplace<c_Transform>(e_camera, c_Transform{});
    entt_registry.emplace<c_Camera>(e_camera, c_Camera{});

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
                "Debug context was not available even when DEBUG flag was "
                "set");
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
        glm::mat4 mat_proj = glm::mat4(1.0f);   // camera -> clip

        const float initial_z = -1.0f;
        mat_view = glm::translate(mat_view, glm::vec3(0.0f, 0.0f, initial_z));

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

            // TODO : This new quatternion shit broke everything (check last
            // commit) Camera updates
            camera_move_system(entt_registry, g4::game_state::input_move,
                               g4::game_state::speed_move);

            camera_look_system(entt_registry, g4::game_state::input_look,
                               g4::game_state::speed_look);
            g4::game_state::input_look = glm::vec2(0.0f, 0.0f);

            // Update zoom
            auto cam_transform = entt_registry.get<c_Transform>(e_camera);
            auto cam_camera = entt_registry.get<c_Camera>(e_camera);

            mat_proj = glm::perspective(glm::radians(cam_camera.fov_deg),
                                        cam_camera.aspect_ratio,
                                        cam_camera.z_near, cam_camera.z_far);
            basic_shader.set_mat4f("uModel", mat_model);
            basic_shader.set_mat4f("uView", cam_transform.view_matrix());
            basic_shader.set_mat4f("uProj", mat_proj);

            triangle_mesh.draw();

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    glfwTerminate();
    return 0;
}