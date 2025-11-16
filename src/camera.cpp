#include "camera.hpp"
#include "logger.hpp"
#include "math.hpp"

#include <format>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "gl_debug.hpp"

// TODO : Reimplement this WIHTOUT using glm::lookAt
Camera::Camera(glm::vec3 initial_world_pos, glm::vec3 initial_world_lookat)
    : m_position(initial_world_pos), m_orientation(1, 0, 0, 0) {
    // // gram-schmidt to build camera basis vectors
    // glm::vec3 forward_world_space =
    //     glm::normalize(initial_world_lookat - initial_world_pos);
    // glm::vec3 right_world_space =
    //     glm::normalize(glm::cross(VEC3_UP_WORLD, forward_world_space));
    // glm::vec3 up_world_space =
    //     glm::normalize(glm::cross(forward_world_space, right_world_space));

    look_at(initial_world_lookat);
}

Camera::~Camera() {}

glm::mat4 Camera::get_view_matrix() const {
    return m_view_matrix;
}

void Camera::look_at(glm::vec3 world_lookat) {
    m_view_matrix = g4::math::look_at(m_position, world_lookat, vec3_up_world);
}

void Camera::set_position(glm::vec3 world_pos) {
    m_position = world_pos;
}