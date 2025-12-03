#pragma once
#include <tuple>
#include <unordered_map>

#include <entt/entt.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/vec3.hpp>

#include "render.hpp"

enum class BlockType {
    AIR,
    DIRT,
};

struct Block {
    BlockType type = BlockType::AIR;
};

constexpr int CHUNK_SIZE = 16;

inline int to_chunk_index(int x, int y, int z) {
    return (z * CHUNK_SIZE * CHUNK_SIZE) + (y * CHUNK_SIZE) + x;
}

inline int to_chunk_index(glm::ivec3 pos) {
    return to_chunk_index(pos.x, pos.y, pos.z);
}

inline std::tuple<int, int, int> to_linear_index_tuple(int index) {
    int z = index / (CHUNK_SIZE * CHUNK_SIZE);
    int y = (index / CHUNK_SIZE) % CHUNK_SIZE;
    int x = index % CHUNK_SIZE;
    return std::make_tuple(x, y, z);
}

inline glm::ivec3 to_linear_index(int index) {
    return glm::ivec3(to_linear_index_tuple(index));
}

inline bool out_of_bounds(int index) {
    return (index < 0) || (index >= (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE));
}

class ChunkArray {
   public:
    inline Block& get_block(int x, int y, int z) const {
        int index = to_chunk_index(x, y, z);
        if (out_of_bounds(index)) {
            return Block{};
        }

        return blocks[index];
    }

    inline bool set_block(int x, int y, int z, Block block) {
        int index = to_chunk_index(x, y, z);
        if (out_of_bounds(index)) {
            return false;
        }

        blocks[index] = block;
        return true;
    }

    inline Block& get_block(glm::ivec3 pos) const { return get_block(pos.x, pos.y, pos.z); }

    inline bool set_block(glm::ivec3 pos, Block block) { return set_block(pos.x, pos.y, pos.z, block); }

   private:
    Block blocks[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE] = {};
};

struct Chunk {
    ChunkArray chunk_array;
    void render(g4::gapi::GraphicsApi& graphics_api, glm::ivec3 chunk_pos) const {
        static gl_MeshResources* block_mesh = nullptr;
        if (block_mesh == nullptr) {
            get_mesh_resources(c_Mesh{// Vertices
                                      {
                                          // Front face
                                          0.0f, 0.0f, 1.0f,  //
                                          1.0f, 0.0f, 1.0f,  //
                                          1.0f, 1.0f, 1.0f,  //
                                          0.0f, 1.0f, 1.0f,  //
                                                             // Back face
                                          0.0f, 0.0f, 0.0f,  //
                                          1.0f, 0.0f, 0.0f,  //
                                          1.0f, 1.0f, 0.0f,  //
                                          0.0f, 1.0f, 0.0f   //
                                      },
                                      // Indices
                                      {// Front face
                                       0, 1, 2, 2, 3, 0,
                                       // Back face
                                       4, 5, 6, 6, 7, 4,
                                       // Left face
                                       4, 0, 3, 3, 7, 4,
                                       // Right face
                                       1, 5, 6, 6, 2, 1,
                                       // Top face
                                       3, 2, 6, 6, 7, 3,
                                       // Bottom face
                                       4, 5, 1, 1, 0, 4}});
        }

        // Render all blocks in chunk
        for (int x = 0; x < CHUNK_SIZE; ++x) {
            for (int y = 0; y < CHUNK_SIZE; ++y) {
                for (int z = 0; z < CHUNK_SIZE; ++z) {
                    Block& block = chunk_array.get_block(x, y, z);
                    if (block.type == BlockType::AIR) {
                        continue;
                    }
                }
            }
        }
    }
};

int vec3_to_chunk_key(glm::ivec3 pos) {
    return (pos.x << 20) | (pos.y << 10) | (pos.z);
}

struct c_Terrian {
    std::unordered_map<glm::ivec3, Chunk, glm::hash<glm::ivec3>> chunks = {};
};

void render_chunk(entt::const Chunk& chunk, g4::gapi::GraphicsApi& graphics_api) {
    void s_ex_render_chunks(entt::registry & registry, g4::gapi::GraphicsApi & graphics_api) {
        auto view = registry.view<c_Terrian>();
        for (auto entity : view) {
            auto& c_terrian = view.get<c_Terrian>(entity);
            for (const auto& [chunk_pos, chunk] : c_terrian.chunks) {
                chunk.render(graphics_api, chunk_pos);
            }
        }
    }
}