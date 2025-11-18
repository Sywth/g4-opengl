#include "camera.hpp"
#include "game_state.hpp"
#include "logger.hpp"
#include "math.hpp"

#include <format>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "gl_debug.hpp"

Camera::Camera(glm::vec3 initial_cam_pos, glm::vec3 initial_cam_target)
    : m_cam_pos(initial_cam_pos),
      m_cam_target(initial_cam_target),
      mn_cam_forward(glm::normalize(initial_cam_target - initial_cam_pos)) {}

Camera::~Camera() {}

// Assume forward does not change (changes target)
void Camera::set_cam_pos(glm::vec3 cam_pos) {
    m_cam_pos = cam_pos;
    m_cam_target = m_cam_pos + mn_cam_forward;
}

// Assumes position does not change (changes target)
void Camera::set_cam_forward(glm::vec3 cam_forward) {
    mn_cam_forward = glm::normalize(cam_forward);
    m_cam_target = m_cam_pos + mn_cam_forward;
}

// Assumes position does not change (changes forward)
void Camera::set_cam_target(glm::vec3 cam_target) {
    m_cam_target = cam_target;
    mn_cam_forward = glm::normalize(m_cam_target - m_cam_pos);
}

void Camera::move_from_input(glm::vec2 input_move, glm::vec2 speed_move) {
    float dt = g4::game_state::delta_time;
    glm::vec3 m_cam_pos_cpy = m_cam_pos;
    m_cam_pos_cpy += get_cam_right() * input_move.x * speed_move.x * dt;
    m_cam_pos_cpy += mn_cam_forward * input_move.y * speed_move.y * dt;

    set_cam_pos(m_cam_pos_cpy);
}

void Camera::rotate_from_input(glm::vec2 input_look, glm::vec2 speed_look) {
    float dt = g4::game_state::delta_time;

    log<LogLevel::Debug>(
        std::format("Input Look: {:.4f}, {:.4f}", input_look.x, input_look.y));

    // 1. Compute yaw (around world-up) and pitch (around camera-right)
    float yaw = input_look.x * speed_look.x * dt * -1.0f;
    float pitch = input_look.y * speed_look.y * dt;

    glm::quat q_yaw = glm::angleAxis(glm::radians(yaw), vec3_up_world);
    glm::quat q_pitch = glm::angleAxis(glm::radians(pitch), get_cam_right());

    glm::quat q = q_yaw * q_pitch;
    glm::vec3 new_forward = q * mn_cam_forward;

    // If its too close to world up vector, reject the pitch
    float dot_up = glm::dot(glm::normalize(new_forward), vec3_up_world);
    if (glm::abs(dot_up) > 0.98f) {
        log<LogLevel::Warn>(
            std::format("Camera pitch too high {:.4f}", dot_up));
        return;
    }

    set_cam_forward(new_forward);
}

glm::vec3 Camera::get_cam_right() const {
    return glm::normalize(glm::cross(mn_cam_forward, vec3_up_world));
}

glm::mat4 Camera::get_view_matrix() const {
    // gives warnign if forward is nearly parallel with world up vector
    if constexpr (debug_enabled) {
        glm::vec3 forward = glm::normalize(m_cam_target - m_cam_pos);
        float dot = glm::abs(glm::dot(forward, vec3_up_world));

        if (dot > 0.99f) {
            log<LogLevel::Warn>(std::format(
                "Camera forward is near parallel with world up! Dot: {:.4f}",
                dot));
        }
    }

    return lookAt(m_cam_pos, m_cam_target, vec3_up_world);
}