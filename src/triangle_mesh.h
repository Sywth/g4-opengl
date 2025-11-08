#pragma once
#include <vector>

class TriangleMesh {
   public:
    TriangleMesh(const std::vector<float>& vertices,
                 const std::vector<unsigned int>& indices);
    ~TriangleMesh();

    void draw() const;

   private:
    unsigned int m_vao, m_vbo, m_ebo;
    unsigned int m_indices_count;
};