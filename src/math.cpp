#include "math.hpp"

namespace g4::math {

// TODO : Check correctness
// Projects vec onto base
inline glm::vec3 proj(glm::vec3 vec, glm::vec3 base) {
    auto vb = glm::dot(vec, base);
    auto bb = glm::dot(base, base);
    return (vb / bb) * base;
}

// TODO : Check correctness
// Runs gram-schmidt to get orthogonal basis
// `normalized_us` : set of normalized vectors (must be linearly independent)
template <size_t M>
std::array<glm::vec3, M> orthonormalize_normalized_vec3s(
    std::array<glm::vec3, M> normalized_us) {
    std::array<glm::vec3, M> normalized_vs;

    for (size_t i = 0; i < normalized_us.size(); i++) {
        glm::vec3 ui = normalized_us[i];

        glm::vec3 subtrahend(0.0f);
        for (size_t j = 0; j < i; j++) {
            glm::vec3 vj = normalized_vs[j];
            subtrahend += proj(ui, vj);
        }

        normalized_vs[i] = ui - subtrahend;
    }
    return normalized_vs;
}

// TODO : Check correctness
glm::mat4 look_at(glm::vec3 loc, glm::vec3 target, glm::vec3 world_up) {
    glm::vec3 cam_forward = glm::normalize(target - loc);
    glm::vec3 cam_right = glm::normalize(glm::cross(cam_forward, world_up));
    glm::vec3 cam_up = glm::normalize(glm::cross(cam_right, cam_forward));

    glm::mat4 look_at = glm::mat4(1.0f);

    // recall glm uses col-row indexing
    look_at[0][0] = cam_right.x;
    look_at[1][0] = cam_right.y;
    look_at[2][0] = cam_right.z;

    look_at[0][1] = cam_up.x;
    look_at[1][1] = cam_up.y;
    look_at[2][1] = cam_up.z;

    look_at[0][2] = -cam_forward.x;
    look_at[1][2] = -cam_forward.y;
    look_at[2][2] = -cam_forward.z;

    return look_at;
}
}  // namespace g4::math