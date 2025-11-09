#include "shader.h"
#include "debug.h"

#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <filesystem>
#include <fstream>
#include <span>

std::string get_file_contents(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " +
                                 std::filesystem::absolute(path).string());
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

unsigned int create_shader_module(unsigned int type,
                                  const std::string& source) {
    const char* source_code_cstr = source.c_str();
    unsigned int shaderId = GL_CALL(glCreateShader(type));
    GL_CALL(glShaderSource(shaderId, 1, &source_code_cstr, nullptr));
    GL_CALL(glCompileShader(shaderId));

    int status;
    GL_CALL(glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status));
    if (!status) {
        char infoLog[MAX_SHADER_INFO_LOG_SIZE];
        GL_CALL(glGetShaderInfoLog(shaderId, MAX_SHADER_INFO_LOG_SIZE, nullptr,
                                   infoLog));
        LOG_ERROR("Shader compilation failed: " + std::string(infoLog));
        GL_CALL(glDeleteShader(shaderId));
        return 0;
    }

    return shaderId;
}

unsigned int create_shader_program(const std::filesystem::path& vertexPath,
                                   const std::filesystem::path& fragmentPath) {
    auto vertexShaderSource = get_file_contents(vertexPath);
    auto fragmentShaderSource = get_file_contents(fragmentPath);

    if (vertexShaderSource.empty() || fragmentShaderSource.empty())
        throw std::runtime_error("Failed to load shader sources.");

    unsigned int vertexShader =
        create_shader_module(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragmentShader =
        create_shader_module(GL_FRAGMENT_SHADER, fragmentShaderSource);

    // log abs path
    LOG_INFO(std::format("Compiling vertex shader @ {}",
                         std::filesystem::absolute(vertexPath).string()));
    LOG_INFO(std::format("Compiling fragment shader @ {}",
                         std::filesystem::absolute(fragmentPath).string()));

    unsigned int shaderProgram = glCreateProgram();
    GL_CALL(glAttachShader(shaderProgram, vertexShader));
    GL_CALL(glAttachShader(shaderProgram, fragmentShader));
    GL_CALL(glLinkProgram(shaderProgram));

    int status;
    GL_CALL(glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status));
    if (!status) {
        char infoLog[MAX_SHADER_INFO_LOG_SIZE];
        GL_CALL(glGetProgramInfoLog(shaderProgram, MAX_SHADER_INFO_LOG_SIZE,
                                    nullptr, infoLog));
        LOG_ERROR("Shader program linking failed: " + std::string(infoLog));
        throw std::runtime_error("Failed to link shader program.");
    }

    GL_CALL(glDeleteShader(vertexShader));
    GL_CALL(glDeleteShader(fragmentShader));

    return shaderProgram;
}

int get_uniform_location(unsigned int programId, const std::string& name) {
    GL_CALL(int location = glGetUniformLocation(programId, name.c_str()));
    if (location == -1) {
        LOG_WARN("Uniform '" + name +
                 "' not found for shader id : " + std::to_string(programId));
    }
    return location;
}

Shader::Shader(const std::filesystem::path& vertexPath,
               const std::filesystem::path& fragmentPath) {
    id = create_shader_program(vertexPath, fragmentPath);
}

Shader::~Shader() {
    GL_CALL(glDeleteProgram(id));
}

void Shader::use() {
    GL_CALL(glUseProgram(id));
}

void Shader::set_bool(const std::string& name, bool value) const {
    auto location = get_uniform_location(id, name);
    GL_CALL(glUniform1i(location, static_cast<int>(value)));
}

void Shader::set_int(const std::string& name, int value) const {
    auto location = get_uniform_location(id, name);
    GL_CALL(glUniform1i(location, value));
}

void Shader::set_float(const std::string& name, float value) const {
    auto location = get_uniform_location(id, name);
    GL_CALL(glUniform1f(location, value));
}

void Shader::set_vecnf(const std::string& name,
                       std::span<const float> values) const {
    auto location = get_uniform_location(id, name);
    switch (values.size()) {
        case 2:
            GL_CALL(glUniform2fv(location, 1, values.data()));
            break;
        case 3:
            GL_CALL(glUniform3fv(location, 1, values.data()));
            break;
        case 4:
            GL_CALL(glUniform4fv(location, 1, values.data()));
            break;
        default:
            throw std::runtime_error(
                "set_vecnf only supports sizes 2, 3, or 4.");
    }
}

// Assumes column major order
void Shader::set_mat4f(const std::string& name, const glm::mat4& matrix) const {
    auto location = get_uniform_location(id, name);
    GL_CALL(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix)));
}