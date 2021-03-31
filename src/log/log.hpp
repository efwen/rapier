#pragma once

#include <string_view>
#include <array>

#include <fmt/format.h>

namespace rp::log {
  enum class Level : uint8_t {
    Trace,
    Info,
    Warn,
    Error,
    ENUM_SIZE,
  };

  constexpr auto horiz_rule = "--------------------------------------------";
  constexpr auto blank_line = "";
  constexpr auto new_line   = "\n";

  void setClientPrefix(std::string_view client_name);
  void logClientMessage(Level log_level, std::string_view format_string, fmt::format_args args);

  template<typename... Args>
  void trace(std::string_view format, const Args&... args) {
    logClientMessage(Level::Trace, format, fmt::make_format_args(args...));
  }

  template<typename... Args>
  void info(std::string_view format, const Args&... args) {
    logClientMessage(Level::Info, format, fmt::make_format_args(args...));
  }

  template<typename... Args>
  void warn(std::string_view format, const Args&... args) {
    logClientMessage(Level::Warn, format, fmt::make_format_args(args...));
  }

  template<typename... Args>
  void error(std::string_view format, const Args&... args) {
    logClientMessage(Level::Error, format, fmt::make_format_args(args...));
  }
}