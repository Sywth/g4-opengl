#include "render.hpp"
#include "gl_debug.hpp"
#include "mesh.hpp"

#include <glad/glad.h>

namespace g4::gapi {
// --- OpenGl Implementation ---
#if defined(USE_GAPI_OPENGL)
GraphicsApi::GraphicsApi() : gl_entity_to_mesh_resources(), gl_uploaded_meshes() {}

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

// TODO: Fix this; this is not RAII compliant (should be defined in some destructor for the resource)
void GraphicsApi::cleanup_mesh(entt::entity entity) {
    auto entity_resources = gl_entity_to_mesh_resources.find(entity);
    bool b_has_resources = (entity_resources != gl_entity_to_mesh_resources.end());
    if (!b_has_resources) {
        return;
    }

    const gl_MeshResources& resources = entity_resources->second;
    GL_CALL(glDeleteBuffers(1, &resources.vbo));
    GL_CALL(glDeleteBuffers(1, &resources.ebo));
    GL_CALL(glDeleteVertexArrays(1, &resources.vao));

    gl_entity_to_mesh_resources.erase(entity_resources);
    gl_uploaded_meshes.erase(entity);
}

GraphicsApi::~GraphicsApi() {
    std::cout << "Cleaning up all mesh resources..." << std::endl;
    for (auto& [entity, resources] : gl_entity_to_mesh_resources) {
        GL_CALL(glDeleteBuffers(1, &resources.vbo));
        GL_CALL(glDeleteBuffers(1, &resources.ebo));
        GL_CALL(glDeleteVertexArrays(1, &resources.vao));
    }

    gl_entity_to_mesh_resources.clear();
    gl_uploaded_meshes.clear();
}

// TODO: Should not take in GraphicsApi instance! This a temporary fix for a fundamental problem :
//  The GraphicsApi class should not exist! The data about vertices etc should exist outside the ECS yes
//  but it should be owned by the ECS via a unique ptr. Each mesh should be refered to via a unique ptr.
void s_ex_render(entt::registry& registry, GraphicsApi& gapi_instance) {
    auto view = registry.view<c_Mesh>();
    for (auto entity : view) {
        auto entity_resources = gapi_instance.gl_entity_to_mesh_resources.find(entity);
        auto entity_uploaded = gapi_instance.gl_uploaded_meshes.find(entity);

        bool b_has_resources = (entity_resources != gapi_instance.gl_entity_to_mesh_resources.end());
        bool b_is_uploaded = (entity_uploaded != gapi_instance.gl_uploaded_meshes.end());

        if (b_has_resources != b_is_uploaded) {
            assert(false && "Inconsistent mesh upload state detected");
        }

        if (!b_is_uploaded || !b_has_resources) {
            // OPT: Is this copy worth it / needed? No right?
            auto mesh = registry.get<c_Mesh>(entity);
            auto uploaded_resources = get_mesh_resources(mesh);
            // Record its uploaded state and corresponding resources
            gapi_instance.gl_entity_to_mesh_resources[entity] = uploaded_resources;
            gapi_instance.gl_uploaded_meshes.insert(entity);
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
GraphicsApi::GraphicsApi() : vk_entity_to_mesh_resources(), vk_uploaded_meshes() {}

gl_MeshResources get_mesh_resources(c_Mesh& mesh) {
    throw std::logic_error("Vulkan support not added yet.");
}

void GraphicsApi::cleanup_mesh(entt::entity entity) {
    throw std::runtime_error("Vulkan support not added yet.");
}

void GraphicsApi::cleanup_all() {
    throw std::runtime_error("Vulkan support not added yet.");
}

// TODO: Should not take in GraphicsApi instance! This a temporary fix for a fundamental problem discussed above
void s_render(entt::registry& registry, GraphicsApi& gapi_instance) {
    throw std::runtime_error("Vulkan support not added yet.");
}
#endif

}  // namespace g4::gapi
