#pragma once

#include "log.hpp"

namespace rp::log {

  enum class Source : uint8_t {
    kEngine = 0,
    kClient,
  };


  void logMessage(Source log_source, Level log_level, std::string_view format_string, fmt::format_args args);
  void logEngineMessage(Level log_level, std::string_view format_string, fmt::format_args args);

  template<typename... Args>
  void rp_trace(std::string_view format, const Args&... args) {
    logEngineMessage(Level::kTrace, format, fmt::make_format_args(args...));
  }

  template<typename... Args>
  void rp_info(std::string_view format, const Args&... args) {
    logEngineMessage(Level::kInfo, format, fmt::make_format_args(args...));
  }

  template<typename... Args>
  void rp_warn(std::string_view format, const Args&... args) {
    logEngineMessage(Level::kWarn, format, fmt::make_format_args(args...));
  }

  template<typename... Args>
  void rp_error(std::string_view format, const Args&... args) {
    logEngineMessage(Level::kError, format, fmt::make_format_args(args...));
  }
}