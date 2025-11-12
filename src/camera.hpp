#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Camera {
   public:
    Camera(glm::vec3 initial_position, glm::vec3 initial_world_lookat);
    ~Camera();

   private:
    glm::vec3 m_position;
    glm::quat m_orientation;
};