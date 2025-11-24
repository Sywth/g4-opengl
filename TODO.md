### Current TODOs

- [ ] Consider moving from macros to compile time polymorphism in [render.hpp](src/render.hpp)
- [ ] Fix and use the namespaces properly and decide how to use them
- [ ] Fix quaterntion drfit / movement issues after repeatly hitting max and min pitch (i.e. shaking mouse fucks up the camera after a bit). Issues is highly likely in [camera_look in camera.cpp](src/camera.cpp)
- [ ] Continue refactoring and fixing main to slowly use ECS
  - [ ] Shader class needs to be an ECS component

### Prev TODOs

- [x] Use stratgey or adapter pattern for graphics back end in [render.hpp](src/render.hpp)

- [x] Fix this new ecs system for camera
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
