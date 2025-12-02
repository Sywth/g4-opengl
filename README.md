# OpenGL G4

A simple OpenGL project for me to test out from. You will also likely need to modifiy [the FindGLFW3.cmake file ](./cmake/FindGLFW3.cmake) (created by JoeyDeVries) to point the correct GLFW install location on your system.

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

- Get [Assimp via vcpkg](https://vcpkg.io/en/package/assimp.html)

  - Run the command in terminal : `.\vcpkg.exe install assimp` as [outlined here](https://vcpkg.io/en/package/assimp.html)
  - This assume you have already installed [vcpkg from this repo](https://github.com/Microsoft/vcpkg)
  - Drag [headers](https://github.com/assimp/assimp/tree/master/include) to `./include/assimp`. You can get this from the source code zip on the [releases page](https://github.com/assimp/assimp/releases)
  - This worked for me; but you might have to build assimp and statically link it if issues arise D:

- Get Entt header from [here](https://github.com/skypjack/entt/tree/master/single_include/entt)

  - Create a folder named `./include/entt` and place the `entt.hpp` file inside it.

- [CMake](https://marketplace.visualstudio.com/items?itemName=twxs.cmake)
- [CMakeTools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)

Misc :

- CPP Standard : 23
- Using the MSVC 2022 amd64 build kit

## Conventions

- `m_` prefix for member variables
- `n_` prefix for member variables that are normalized

- `c_` prefix for ECS component (struct and var)
- `e_` prefix for ECS entity id (var)
- `s_` prefix for ECS system (function name (only takes registry as arg))
- `s_ex_` prefix for ECS system that takes external input (e.g. renderer might take an asset manager also)

- `FIXME: ` - When code causes critical issues (i.e. causes compile errors or crashes)
- `TODO: ` - Modifications / addition that need to be
- `NOTE: ` - Notes explaining unintuitive parts of the code
- `NOTSURE: ` - Note sure (this to avoid cargo culting while also making fast progress)
- `OPT: ` - Possible optimizations
- `REFACTOR: ` - Refactor this, its ugly
- `FUTURE: ` - Unimplemented place holder to be done in future (if ever)
