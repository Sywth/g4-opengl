#include "game_state.hpp"

glm::vec3 g4::game_state::clear_color{0.2f, 0.3f, 0.3f};
glm::vec2 g4::game_state::input_move{0.0f, 0.0f};
glm::vec2 g4::game_state::input_look{0.0f, 0.0f};
glm::vec2 g4::game_state::speed_move{10.f, 10.f};
glm::vec2 g4::game_state::speed_look{100.f, 100.f};

float g4::game_state::delta_time = 0.0f;
float g4::game_state::time_last_frame = 0.0f;
