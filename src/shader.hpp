#pragma once
#include "logging.hpp"

#include <glm/glm.hpp>

#include <filesystem>
#include <span>
#include <string>

const int MAX_SHADER_INFO_LOG_SIZE = 1024;
std::string get_file_contents(const std::filesystem::path& path);
unsigned int create_shader_module(unsigned int type, const std::string& source);
unsigned int create_shader_program(const std::filesystem::path& vertexPath,
                                   const std::filesystem::path& fragmentPath);

class Shader {
   public:
    unsigned int id;
    Shader(const std::filesystem::path& vertexPath,
           const std::filesystem::path& fragmentPath);
    ~Shader();

    void use();
    void set_bool(const std::string& name, bool value) const;
    void set_int(const std::string& name, int value) const;
    void set_float(const std::string& name, float value) const;
    void set_vecnf(const std::string& name,
                   std::span<const float> values) const;
    void set_mat4f(const std::string& name, const glm::mat4& matrix) const;
};