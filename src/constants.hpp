#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

namespace g4 {
namespace config {
namespace display {
inline constexpr float gamma = 2.2f;
inline constexpr float z_near = 0.1f;
inline constexpr float z_far = 100.0f;
}  // namespace display
}  // namespace config

inline constexpr glm::vec3 vec3_up_world{0.0f, 1.0f, 0.0f};
inline constexpr glm::vec3 vec3_right_world{1.0f, 0.0f, 0.0f};
inline constexpr glm::vec3 vec3_forward_world{0.0f, 0.0f, -1.0f};
}  // namespace g4
