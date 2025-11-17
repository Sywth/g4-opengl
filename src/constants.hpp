#pragma once

namespace g4 {

// --- configurable values ---
namespace config {
namespace display {
inline constexpr int width = 1280;
inline constexpr int height = 720;
inline constexpr float gamma = 2.2f;
inline constexpr float z_near = 0.1f;
inline constexpr float z_far = 100.0f;
}  // namespace display
}  // namespace config

// --- derived values ---
//  Don't add constant values here
namespace config {
namespace display {
inline constexpr float aspect_ratio =
    static_cast<float>(width) / static_cast<float>(height);
}
}  // namespace config

}  // namespace g4
