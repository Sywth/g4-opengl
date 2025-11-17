// TODO : This is temporary; in future refactor this (should avoid global state
// / singletons when possible)
#pragma once

#include <glm/glm.hpp>

namespace g4 {
namespace game_state {
glm::vec3 clear_color(0.2f, 0.3f, 0.3f);

glm::vec2 input_move(0.0f, 0.0f);
glm::vec2 input_look(0.0f, 0.0f);

glm::vec2 speed_move(0.005f, 0.005f);
glm::vec2 speed_look(0.005f, 0.005f);
}  // namespace game_state
}  // namespace g4