#pragma once

#include "log/log.hpp"

namespace rp::log {

  enum class Source : uint8_t {
    Engine,
    Client,
  };


  void logMessage(Source log_source, Level log_level, std::string_view format_string, fmt::format_args args);
  void logEngineMessage(Level log_level, std::string_view format_string, fmt::format_args args);

  template<typename... Args>
  void rp_trace(std::string_view format, const Args&... args) {
    logEngineMessage(Level::Trace, format, fmt::make_format_args(args...));
  }

  template<typename... Args>
  void rp_info(std::string_view format, const Args&... args) {
    logEngineMessage(Level::Info, format, fmt::make_format_args(args...));
  }

  template<typename... Args>
  void rp_warn(std::string_view format, const Args&... args) {
    logEngineMessage(Level::Warn, format, fmt::make_format_args(args...));
  }

  template<typename... Args>
  void rp_error(std::string_view format, const Args&... args) {
    logEngineMessage(Level::Error, format, fmt::make_format_args(args...));
  }
}