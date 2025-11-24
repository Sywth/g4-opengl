#include "render.hpp"
#include "gl_debug.hpp"
#include "mesh.hpp"

#include <glad/glad.h>

namespace g4::gapi {

// --- OpenGl Implementation ---
#if defined(USE_GAPI_OPENGL)
std::unordered_map<entt::entity, gl_MeshResources> GraphicsApi::gl_entity_to_mesh_resources;
std::unordered_set<entt::entity> GraphicsApi::gl_uploaded_meshes;

gl_MeshResources get_mesh_resources(c_Mesh& mesh) {
    gl_MeshResources resources{};

    GL_CALL(glGenVertexArrays(1, &resources.vao));
    GL_CALL(glGenBuffers(1, &resources.vbo));
    GL_CALL(glGenBuffers(1, &resources.ebo));

    GL_CALL(glBindVertexArray(resources.vao));

    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, resources.vbo));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(float), mesh.vertices.data(), GL_STATIC_DRAW));

    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, resources.ebo));
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(),
                         GL_STATIC_DRAW));

    resources.indices_count = static_cast<unsigned int>(mesh.indices.size());

    GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));
    GL_CALL(glEnableVertexAttribArray(0));

    GL_CALL(glBindVertexArray(0));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    return resources;
}

void GraphicsApi::cleanup_mesh(entt::entity entity) {
    auto entity_resources = GraphicsApi::gl_entity_to_mesh_resources.find(entity);
    bool b_has_resources = (entity_resources != GraphicsApi::gl_entity_to_mesh_resources.end());
    if (!b_has_resources) {
        return;
    }

    const gl_MeshResources& resources = entity_resources->second;
    GL_CALL(glDeleteBuffers(1, &resources.vbo));
    GL_CALL(glDeleteBuffers(1, &resources.ebo));
    GL_CALL(glDeleteVertexArrays(1, &resources.vao));

    GraphicsApi::gl_entity_to_mesh_resources.erase(entity_resources);
    GraphicsApi::gl_uploaded_meshes.erase(entity);
}

void GraphicsApi::cleanup_all() {
    for (auto& [entity, resources] : GraphicsApi::gl_entity_to_mesh_resources) {
        GL_CALL(glDeleteBuffers(1, &resources.vbo));
        GL_CALL(glDeleteBuffers(1, &resources.ebo));
        GL_CALL(glDeleteVertexArrays(1, &resources.vao));
    }

    GraphicsApi::gl_entity_to_mesh_resources.clear();
    GraphicsApi::gl_uploaded_meshes.clear();
}

void s_render(entt::registry& registry) {
    auto view = registry.view<c_Mesh>();
    for (auto entity : view) {
        auto entity_resources = GraphicsApi::gl_entity_to_mesh_resources.find(entity);
        auto entity_uploaded = GraphicsApi::gl_uploaded_meshes.find(entity);

        bool b_has_resources = (entity_resources != GraphicsApi::gl_entity_to_mesh_resources.end());
        bool b_is_uploaded = (entity_uploaded != GraphicsApi::gl_uploaded_meshes.end());
        if (b_has_resources != b_is_uploaded) {
            assert(false && "Inconsistent mesh upload state detected");
        }

        if (!b_is_uploaded || !b_has_resources) {
            // OPT: Is this copy worth it / needed? No right?
            auto mesh = registry.get<c_Mesh>(entity);
            auto uploaded_resources = get_mesh_resources(mesh);
            // Record its uploaded state and corresponding resources
            GraphicsApi::gl_entity_to_mesh_resources[entity] = uploaded_resources;
            GraphicsApi::gl_uploaded_meshes.insert(entity);
            continue;
        }

        const gl_MeshResources& resources = entity_resources->second;
        GL_CALL(glBindVertexArray(resources.vao));
        GL_CALL(glDrawElements(GL_TRIANGLES, resources.indices_count, GL_UNSIGNED_INT, 0));
        GL_CALL(glBindVertexArray(0));
    }
}
#endif

// --- Vulkan Implementation ---
#if defined(USE_GAPI_VULKAN)
std::unordered_map<entt::entity, vk_MeshResources> GraphicsApi::vk_entity_to_mesh_resources;
std::unordered_set<entt::entity> GraphicsApi::vk_uploaded_meshes;

gl_MeshResources get_mesh_resources(c_Mesh& mesh) {
    throw std::logic_error("Vulkan support not added yet.");
}

void GraphicsApi::cleanup_mesh(entt::entity entity) {
    throw std::runtime_error("Vulkan support not added yet.");
}

void GraphicsApi::cleanup_all() {
    throw std::runtime_error("Vulkan support not added yet.");
}

void s_render(entt::registry& registry) {
    throw std::runtime_error("Vulkan support not added yet.");
}
#endif

GraphicsApi& get_gapi_instance() {
    static GraphicsApi instance;
    return instance;
}
}  // namespace g4::gapi
