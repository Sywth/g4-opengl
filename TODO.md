- [ ] Fix this new ecs system for camera
  - [ ] Wtf happened to the movment and looking system
  - [ ] I doubt its speed i think its fucky with the quaternions or matrix math
- [ ] Make everything ecs

- [ ] Fix the current camera movement

  - [ ] Instead of storing forward, target, position just store a transform object which as a vec3 positio and quat orientation
  - [ ] The current use of globals in g4::game_state is clunky and already has lead to multiple issues to do with figuring out where state is being mutated
  - [ ] It might be smoother to ditch updating camera look update from callback and instead do it in the main game loop like movement

- [ ] Finish step https://learnopengl.com/Getting-started/Camera and really understand the gram-schmidt process and try using that instead of just using glm::lookAt
- [ ] Add camera movement
- [ ] Make current logger.hpp more configurable for user (i.e. we shouldn't have to go into the header to change the log level)

- [x] Modifiy `./src/gl_debug.hpp` to not use any macros; prefer `constexpr` and `template` techniques
- [x] Modifiy camera code to be better designed; have less state and modifications and actually use the quaternion orientation
