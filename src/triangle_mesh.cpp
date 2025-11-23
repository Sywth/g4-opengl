#include "triangle_mesh.hpp"
#include "gl_debug.hpp"

#include <glad/glad.h>
#include <cassert>

TriangleMesh::TriangleMesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices)
    : m_indices_count(static_cast<unsigned int>(indices.size())) {
    GL_CALL(glGenVertexArrays(1, &m_vao));
    GL_CALL(glGenBuffers(1, &m_vbo));
    GL_CALL(glGenBuffers(1, &m_ebo));

    // Bind VAO before EBO
    GL_CALL(glBindVertexArray(m_vao));

    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW));

    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo));
    GL_CALL(
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW));

    //  TODO : understand what binds to the vao
    //      - why does glVertexAttribPointer(0) and glEnableVertexAttribArray(0)
    //      have 0?
    //      - how do different attributes (e.g. add color and flag, how does
    //      that change?)
    GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));
    GL_CALL(glEnableVertexAttribArray(0));

    // Unbind VAO before EBO, VBO (otherwise you'd be binding to the VAO again)
    GL_CALL(glBindVertexArray(0));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

TriangleMesh::~TriangleMesh() {
    GL_CALL(glDeleteBuffers(1, &m_vbo));
    GL_CALL(glDeleteBuffers(1, &m_ebo));
    GL_CALL(glDeleteVertexArrays(1, &m_vao));
}

void TriangleMesh::draw() const {
    GL_CALL(glBindVertexArray(m_vao));
    GL_CALL(glDrawElements(GL_TRIANGLES, m_indices_count, GL_UNSIGNED_INT, 0));
    GL_CALL(glBindVertexArray(0));
}