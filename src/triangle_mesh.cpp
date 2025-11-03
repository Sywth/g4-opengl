#include "triangle_mesh.h"
#include <glad/glad.h>
#include "debug.h"

TriangleMesh::TriangleMesh(const std::vector<float>& vertices,
                           const std::vector<unsigned int>& indices,
                           unsigned int shader_program)
    : m_shader_program(shader_program),
      m_indices_count(static_cast<unsigned int>(indices.size())) {
    GL_CALL(glGenVertexArrays(1, &m_vao));
    GL_CALL(glBindVertexArray(m_vao));

    GL_CALL(glGenBuffers(1, &m_vbo));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
                         vertices.data(), GL_STATIC_DRAW));

    GL_CALL(glGenBuffers(1, &m_ebo));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo));
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         indices.size() * sizeof(unsigned int), indices.data(),
                         GL_STATIC_DRAW));

    GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                                  (void*)0));
    GL_CALL(glEnableVertexAttribArray(0));

    GL_CALL(glBindVertexArray(0));
}

TriangleMesh::~TriangleMesh() {
    GL_CALL(glDeleteBuffers(1, &m_vbo));
    GL_CALL(glDeleteBuffers(1, &m_ebo));
    GL_CALL(glDeleteVertexArrays(1, &m_vao));
}

void TriangleMesh::draw() const {
    GL_CALL(glUseProgram(m_shader_program));
    GL_CALL(glBindVertexArray(m_vao));
    GL_CALL(glDrawElements(GL_TRIANGLES, m_indices_count, GL_UNSIGNED_INT, 0));
    GL_CALL(glBindVertexArray(0));
}