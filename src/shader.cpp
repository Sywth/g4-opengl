#include "shader.h"
#include <glad/glad.h>
#include <filesystem>
#include <fstream>
#include "debug.h"

const char* get_file_contents(const std::filesystem::path path) {
  static std::vector<char> buffer;

  std::ifstream file(path, std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    LOG_WARN("Failed to open file: " +
             std::filesystem::absolute(path).string());
    return nullptr;
  }

  std::streamsize size = file.tellg();
  if (size < 0) {
    LOG_WARN("Failed to get file size for: " +
             std::filesystem::absolute(path).string());
    return nullptr;
  }
  file.seekg(0, std::ios::beg);

  buffer.resize(static_cast<size_t>(size) + 1);
  if (!file.read(buffer.data(), size)) {
    LOG_WARN("Failed to read file: " +
             std::filesystem::absolute(path).string());
    return nullptr;
  }

  buffer[size] = '\0';
  return buffer.data();
}

unsigned int create_shader_module(unsigned int type, const char* source) {
  unsigned int shaderId = GL_CALL(glCreateShader(type));
  GL_CALL(glShaderSource(shaderId, 1, &source, nullptr));
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

  if (!vertexShaderSource || !fragmentShaderSource)
    throw std::runtime_error("Failed to load shader sources.");

  unsigned int vertexShader =
      create_shader_module(GL_VERTEX_SHADER, vertexShaderSource);
  unsigned int fragmentShader =
      create_shader_module(GL_FRAGMENT_SHADER, fragmentShaderSource);
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
