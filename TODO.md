- [ ] Fix the current camera movement

  - [ ] The current use of globals in g4::game_state is clunky and already has lead to multiple issues to do with figuring out where state is being mutated
  - [ ] It might be smoother to ditch updating camera look update from callback and instead do it in the main game loop like movement

- [ ] Finish step https://learnopengl.com/Getting-started/Camera and really understand the gram-schmidt process and try using that instead of just using glm::lookAt
- [ ] Add camera movement
- [ ] Make current logger.hpp more configurable for user (i.e. we shouldn't have to go into the header to change the log level)

- [x] Modifiy `./src/gl_debug.hpp` to not use any macros; prefer `constexpr` and `template` techniques
- [x] Modifiy camera code to be better designed; have less state and modifications and actually use the quaternion orientation
