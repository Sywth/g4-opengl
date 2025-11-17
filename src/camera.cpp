#include "camera.hpp"
#include "logger.hpp"
#include "math.hpp"

#include <format>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "gl_debug.hpp"
Camera::Camera(glm::vec3 initial_world_pos, glm::vec3 initial_world_target)
    : m_world_pos(initial_world_pos),
      m_world_target(initial_world_target),
      m_world_forward(
          glm::normalize(initial_world_target - initial_world_pos)) {}

Camera::~Camera() {}

void Camera::set_world_pos(glm::vec3 world_pos) {
    m_world_pos = world_pos;
    m_world_target = m_world_pos + m_world_forward;
}

void Camera::set_world_forward(glm::vec3 world_forward) {
    m_world_forward = glm::normalize(world_forward);
    m_world_target = m_world_pos + m_world_forward;
}

void Camera::set_world_target(glm::vec3 world_target) {
    m_world_target = world_target;
    m_world_forward = glm::normalize(m_world_target - m_world_pos);
}

void Camera::move_from_input(glm::vec2 input_move, glm::vec2 speed_move) {
    glm::vec3 right =
        glm::normalize(glm::cross(m_world_forward, vec3_up_world));
    glm::vec3 up = glm::normalize(glm::cross(right, m_world_forward));

    m_world_pos += right * input_move.x * speed_move.x;
    m_world_pos += up * input_move.y * speed_move.y;

    m_world_target = m_world_pos + m_world_forward;
}

glm::mat4 Camera::get_view_matrix() const {
    // gives warnign if forward is nearly parallel with world up vector
    if constexpr (debug_enabled) {
        glm::vec3 forward = glm::normalize(m_world_target - m_world_pos);
        float dot = glm::abs(glm::dot(forward, vec3_up_world));

        if (dot > 0.99f) {
            log<LogLevel::Warn>(std::format(
                "Camera forward is near parallel with world up! Dot: {:.4f}",
                dot));
        }
    }

    return lookAt(m_world_pos, m_world_target, vec3_up_world);
}