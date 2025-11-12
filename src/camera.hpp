#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

inline constexpr glm::vec3 VEC3_UP_WORLD{0.0f, 1.0f, 0.0f};
inline constexpr glm::vec3 VEC3_RIGHT_WORLD{1.0f, 0.0f, 0.0f};
inline constexpr glm::vec3 VEC3_FORWARD_WORLD{0.0f, 0.0f, -1.0f};

class Camera {
   public:
    Camera(glm::vec3 initial_world_pos, glm::vec3 initial_world_lookat);
    ~Camera();

    glm::mat4 get_view_matrix() const;
    void look_at(glm::vec3 world_lookat);
    void set_position(glm::vec3 world_pos);

   private:
    glm::vec3 m_position;
    glm::quat m_orientation;
    glm::mat4 m_view_matrix;
};