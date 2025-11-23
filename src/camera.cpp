#include "camera.hpp"

void camera_move(c_Transform& transform, float dt, const glm::vec2& input_move, const glm::vec2& speed_move) {
    glm::vec3 forward = transform.forward();
    glm::vec3 right = transform.right();

    transform.position += forward * input_move.y * speed_move.y * dt;
    transform.position += right * input_move.x * speed_move.x * dt;
}

void camera_look(c_Transform& transform, float dt, const glm::vec2& input_look, const glm::vec2& speed_look) {
    float yaw = input_look.x * speed_look.x * dt * -1.0f;
    float pitch = input_look.y * speed_look.y * dt;

    // TODO : Remove the radian conversion
    glm::quat q_yaw = glm::angleAxis(glm::radians(yaw), g4::vec3_up_world);
    glm::quat q_pitch = glm::angleAxis(glm::radians(pitch), transform.right());

    // FIXME: This still cause my left and right keys to drift
    // Apply yaw first (in world space) then pitch (in local space)
    glm::quat new_rot = q_pitch * (q_yaw * transform.rotation);

    float dot_up = glm::dot(glm::rotate(new_rot, g4::vec3_forward_world), g4::vec3_up_world);
    if (glm::abs(dot_up) > 0.9f) {
        log<LogLevel::Debug>("Pitch limit reached");
        new_rot = q_yaw * transform.rotation;
    }

    // NS: Normalize again to avoid drift
    transform.rotation = glm::normalize(new_rot);
}

void s_camera(entt::registry& registry) {
    float dt = g4::game_state::delta_time;
    glm::vec2 input_move = g4::game_state::input_move;
    glm::vec2 speed_move = g4::game_state::speed_move;
    glm::vec2 input_look = g4::game_state::input_look;
    glm::vec2 speed_look = g4::game_state::speed_look;

    auto view = registry.view<c_Transform, c_Camera>();
    for (auto entity : view) {
        c_Transform& transform = view.get<c_Transform>(entity);

        camera_move(transform, dt, input_move, speed_move);
        camera_look(transform, dt, input_look, speed_look);
    }
}