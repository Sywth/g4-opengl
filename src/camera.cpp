#define _LOG_LEVEL_DEBUG
#define _LOG_FLUSH

#include "camera.hpp"
#include "logging.hpp"

#include <format>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "debug.hpp"

Camera::Camera(glm::vec3 initial_position, glm::vec3 initial_world_lookat)
    : m_position(initial_position), m_orientation(glm::quat()) {
    std::cout << "Hello from Camera constructor" << std::endl;
    log<LogLevel::Debug>("Hello from Camera constructor");
    log<LogLevel::Debug>(std::format("Quat : <{:.2}, {:.2}, {:.2}, {:.2}>",
                                     m_orientation.w, m_orientation.x,
                                     m_orientation.y, m_orientation.z));
}

Camera::~Camera() {}