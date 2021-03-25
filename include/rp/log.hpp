#pragma once

#include <string_view>
#include <array>

#include <fmt/format.h>

namespace rp::log {
  enum class Level : uint8_t {
    kTrace = 0,
    kInfo,
    kWarn,
    kError,
    kSize,
  };

  void logClientMessage(Level log_level, std::string_view format_string, fmt::format_args args);

  template<typename... Args>
  void trace(std::string_view format, const Args&... args) {
    logClientMessage(Level::kTrace, format, fmt::make_format_args(args...));
  }

  template<typename... Args>
  void info(std::string_view format, const Args&... args) {
    logClientMessage(Level::kInfo, format, fmt::make_format_args(args...));
  }

  template<typename... Args>
  void warn(std::string_view format, const Args&... args) {
    logClientMessage(Level::kWarn, format, fmt::make_format_args(args...));
  }

  template<typename... Args>
  void error(std::string_view format, const Args&... args) {
    logClientMessage(Source::kClient, Level::kError, format, fmt::make_format_args(args...));
  }
}