#include "shader.h"
#include <glad/glad.h>
#include <filesystem>
#include <fstream>
#include "debug.h"

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
