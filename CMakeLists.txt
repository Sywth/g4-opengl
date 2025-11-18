cmake_minimum_required(VERSION 3.26)

project(g4 VERSION 0.0.1)
cmake_policy(SET CMP0072 NEW)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
# find_package(assimp REQUIRED) # TODO Figure out at some point why this breaks cmake 

# Compiles all files in ./src automatically
file(GLOB_RECURSE SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# You must manually add vendor files here
set(VENDOR_FILES
    vendor/glad.c
    # vendor/entt.hpp # Like this?
)

add_executable(g4
    ${SRC_FILES}
    ${VENDOR_FILES}
)


target_include_directories(g4 PRIVATE dependencies include)
target_link_libraries(
    g4 ${GLFW3_LIBRARY} 
    OpenGL::GL
)
