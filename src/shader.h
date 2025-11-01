#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <memory>
#include "logging.h"

const int MAX_SHADER_INFO_LOG_SIZE = 1024;
const char *get_file_contents(const std::filesystem::path path);
unsigned int create_shader_module(unsigned int type, const char *source);
unsigned int create_shader_program(const std::filesystem::path &vertexPath, const std::filesystem::path &fragmentPath);
