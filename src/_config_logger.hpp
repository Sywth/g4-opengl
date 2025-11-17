#pragma once
enum class LogLevel { Debug, Info, Warn, Error, Fatal, None };

constexpr LogLevel current_log_level = LogLevel::Debug;
constexpr bool flush_log = true;
