#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <entt/entt.hpp>

#include "game_state.hpp"
#include "logger.hpp"
#include "transform.hpp"

struct c_Camera {
    float fov_deg{64.0f};
    float z_near{0.0625f};
    float z_far{256.0f};
    float aspect_ratio{static_cast<float>(g4::game_state::width) / static_cast<float>(g4::game_state::height)};
};

void s_camera(entt::registry& registry);