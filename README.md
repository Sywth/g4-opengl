# OpenGL G4

A simple OpenGL project for me to test out from. You will also likely need to modifiy [the FindGLFW3.cmake file ](./cmake/FindGLFW3.cmake) (created by JoeyDeVries) to point the

Note : Only works when running from CMake extension panel in VSCode for me personally.

Dependencies :

- Get [Glad from here](https://glad.dav1d.de/)

  - Language : C/C++
  - Specification : OpenGL
  - API : gl version 4.3 (rest None)
  - Profile : Core
  - Generate a loader : Yes
  - Extensions : None
  - Skip anything not mentioned (basically default which is none / don't include)

- Get [GLFW from here](https://www.glfw.org/download.html)

  - Personally using 64-bit Windows binaries

- Get [GLM from here](https://github.com/g-truc/glm/releases)
  - Place the glm folder inside the zip inside ./include

Extensions :

- [CMake](https://marketplace.visualstudio.com/items?itemName=twxs.cmake)
- [CMakeTools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)

Misc :

- CPP Standard : 23
- Using the MSVC 2022 amd64 build kit
