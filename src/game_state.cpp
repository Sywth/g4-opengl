#include "game_state.hpp"

constexpr double input_look_speed = 1e-8;
constexpr double input_move_speed = 1e-6;

glm::vec3 g4::game_state::clear_color{0.2f, 0.3f, 0.3f};
glm::vec2 g4::game_state::input_move{input_move_speed};
glm::vec2 g4::game_state::input_look{input_look_speed};
glm::vec2 g4::game_state::speed_move{10.f, 10.f};
glm::vec2 g4::game_state::speed_look{100.f, 100.f};
glm::vec2 g4::game_state::mouse_sensitivity{0.1f, 0.1f};

float g4::game_state::fov = 70.0f;
float g4::game_state::delta_time = 0.0f;
float g4::game_state::time_last_frame = 0.0f;
