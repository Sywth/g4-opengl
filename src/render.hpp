#pragma once

#include <entt/entt.hpp>
#include <unordered_map>

// RE: OPT: Re-do this with strategy pattern / adapter pattern
namespace g4::gapi {
// --- Data for graphics APIs ---
struct gl_MeshResources {
    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;
    unsigned int indices_count;
};

struct vk_MeshResources {
    // FUTURE: Add Vulkan mesh resources here
};

// --- Graphics APIs ---
class GraphicsApi {
   public:
    // TODO: Consider using to called cleanup_all automatically at destruction
    virtual ~GraphicsApi() = default;

    virtual void cleanup_mesh(entt::entity entity) = 0;
    virtual void cleanup_all() = 0;
};

class gl_GraphicsApi : public GraphicsApi {
   public:
    void cleanup_mesh(entt::entity entity) override;
    void cleanup_all() override;
    static std::unordered_map<entt::entity, gl_MeshResources> entity_to_mesh_resources;
    static std::unordered_set<entt::entity> uploaded_meshes;
};
gl_GraphicsApi& get_gl_graphics_api_instance();

class vk_GraphicsApi : public GraphicsApi {
   public:
    void cleanup_mesh(entt::entity entity) override;
    void cleanup_all() override;
    static std::unordered_map<entt::entity, vk_MeshResources> entity_to_mesh_resources;
    static std::unordered_set<entt::entity> uploaded_meshes;
};

void s_gl_render(entt::registry& registry);
void s_vk_render(entt::registry& registry);
vk_GraphicsApi& get_vk_graphics_api_instance();

}  // namespace g4::gapi