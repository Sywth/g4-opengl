#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

inline constexpr glm::vec3 vec3_up_world{0.0f, 1.0f, 0.0f};
inline constexpr glm::vec3 vec3_right_world{1.0f, 0.0f, 0.0f};
inline constexpr glm::vec3 vec3_forward_world{0.0f, 0.0f, -1.0f};

class Camera {
   public:
    Camera(glm::vec3 initial_world_pos, glm::vec3 initial_world_target);
    ~Camera();

    glm::mat4 get_view_matrix() const;

    void set_world_pos(glm::vec3 world_pos);
    void set_world_forward(glm::vec3 world_forward);
    void set_world_target(glm::vec3 world_target);

   private:
    glm::vec3 m_world_pos;
    glm::vec3 m_world_target;
    glm::vec3 m_world_forward;
};