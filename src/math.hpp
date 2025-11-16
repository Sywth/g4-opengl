#pragma once
#include <array>
#include <glm/glm.hpp>

namespace g4::math {
inline glm::vec3 proj(glm::vec3 vec, glm::vec3 base);

template <size_t M>
std::array<glm::vec3, M> orthonormalize_normalized_vec3s(
    std::array<glm::vec3, M> normalized_us);

glm::mat4 look_at(glm::vec3 loc, glm::vec3 target, glm::vec3 world_up);
}  // namespace g4::math