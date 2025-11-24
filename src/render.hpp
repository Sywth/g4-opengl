#pragma once

#include "_config_graphics.hpp"

#include <entt/entt.hpp>
#include <unordered_map>

// REFACTOR: There might be a better way to do this; but atm i cba and i dont think its worth
// technical debt. Yes this should be a simple interface where we define functions but C++ is
// not a simple langauge. Hence we define our interface via macros. It would be easier to use class
// but classes are the devil.
// Things ive tried but dont fit here:
// 1. Startgey pattern: Runtime depedent hence we'd have a bunch of useless opengl / vulkan code after compilation
// 2. Templates: Way too much bloat and i couldnt find a way to get everything i wanted to working
// 3. ABC with tempaltes: Just weird and to complex

// Maybe consider compile time polymorphism
//  in fact this video  [Compile-time vs run-time polymorphism with C++](https://www.youtube.com/watch?v=q0yyIkCn-5U)
//  might be just what we need.
namespace g4::gapi {

#if defined(USE_GAPI_OPENGL)
struct gl_MeshResources {
    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;
    unsigned int indices_count;
};
#endif

#if defined(USE_GAPI_VULKAN)
struct vk_MeshResources {};
#endif

class GraphicsApi {
   public:
    void cleanup_mesh(entt::entity entity);
    void cleanup_all();

#if defined(USE_GAPI_OPENGL)
    static std::unordered_map<entt::entity, gl_MeshResources> gl_entity_to_mesh_resources;
    static std::unordered_set<entt::entity> gl_uploaded_meshes;
#endif

#if defined(USE_GAPI_VULKAN)
    static std::unordered_map<entt::entity, vk_MeshResources> vk_entity_to_mesh_resources;
    static std::unordered_set<entt::entity> vk_uploaded_meshes;
#endif
};

GraphicsApi& get_gapi_instance();
void s_render(entt::registry& registry);
}  // namespace g4::gapi