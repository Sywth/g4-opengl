#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "constants.hpp"

struct c_Transform {
    glm::vec3 position{0.0f};
    glm::vec3 scale{1.0f};
    glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};

    glm::vec3 forward() const { return glm::rotate(rotation, g4::vec3_forward_world); }
    glm::vec3 right() const { return glm::rotate(rotation, g4::vec3_right_world); }
    glm::vec3 up() const { return glm::rotate(rotation, g4::vec3_up_world); }

    glm::mat4 view_matrix() const { return glm::lookAt(position, position + forward(), g4::vec3_up_world); }
};
