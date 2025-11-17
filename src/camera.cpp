#include "camera.hpp"
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
      m_cam_forward(glm::normalize(initial_cam_target - initial_cam_pos)) {}

Camera::~Camera() {}

void Camera::set_cam_pos(glm::vec3 cam_pos) {
    m_cam_pos = cam_pos;
    m_cam_target = m_cam_pos + m_cam_forward;
}

void Camera::set_cam_forward(glm::vec3 cam_forward) {
    m_cam_forward = glm::normalize(cam_forward);
    m_cam_target = m_cam_pos + m_cam_forward;
}

void Camera::set_cam_target(glm::vec3 cam_target) {
    m_cam_target = cam_target;
    m_cam_forward = glm::normalize(m_cam_target - m_cam_pos);
}

glm::vec3 Camera::get_cam_right() const {
    return glm::normalize(glm::cross(m_cam_forward, vec3_up_world));
}

void Camera::move_from_input(glm::vec2 input_move, glm::vec2 speed_move) {
    glm::vec3 m_cam_pos_cpy = m_cam_pos;
    m_cam_pos_cpy += get_cam_right() * input_move.x * speed_move.x;
    m_cam_pos_cpy += m_cam_forward * input_move.y * speed_move.y;

    set_cam_pos(m_cam_pos_cpy);
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