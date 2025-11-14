#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

inline constexpr glm::vec3 vec3_up_world{0.0f, 1.0f, 0.0f};
inline constexpr glm::vec3 vec3_right_world{1.0f, 0.0f, 0.0f};
inline constexpr glm::vec3 vec3_forward_world{0.0f, 0.0f, -1.0f};

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