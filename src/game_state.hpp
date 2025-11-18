// TODO : This is temporary; in future refactor this (should avoid global state
// / singletons when possible)
#pragma once

#include <glm/glm.hpp>

namespace g4 {
namespace game_state {
extern glm::vec3 clear_color;
extern glm::vec2 input_move;
extern glm::vec2 input_look;
extern glm::vec2 speed_move;
extern glm::vec2 speed_look;
extern glm::vec2 mouse_sensitivity;

extern float fov;
extern float delta_time;
extern float time_last_frame;
}  // namespace game_state
}  // namespace g4