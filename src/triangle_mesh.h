#pragma once
#include <vector>

class TriangleMesh {
 public:
  TriangleMesh(const std::vector<float>& vertices,
               const std::vector<unsigned int>& indices,
               unsigned int shader_program);
  ~TriangleMesh();

  void Draw() const;

 private:
  unsigned int m_vao, m_vbo, m_ebo, m_shader_program, m_indices_count;
};