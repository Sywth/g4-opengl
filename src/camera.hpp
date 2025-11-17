#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

inline constexpr glm::vec3 vec3_up_world{0.0f, 1.0f, 0.0f};
inline constexpr glm::vec3 vec3_right_world{1.0f, 0.0f, 0.0f};
inline constexpr glm::vec3 vec3_forward_world{0.0f, 0.0f, -1.0f};

class Camera {
   public:
    Camera(glm::vec3 initial_cam_pos, glm::vec3 initial_cam_target);
    ~Camera();

    glm::mat4 get_view_matrix() const;

    void set_cam_pos(glm::vec3 cam_pos);
    void set_cam_forward(glm::vec3 cam_forward);
    void set_cam_target(glm::vec3 cam_target);

    glm::vec3 get_cam_right() const;
    void move_from_input(glm::vec2 input_move, glm::vec2 speed_move);

   private:
    glm::vec3 m_cam_pos;
    glm::vec3 m_cam_target;
    glm::vec3 m_cam_forward;
};