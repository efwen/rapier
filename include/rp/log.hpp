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

  namespace internal {
    enum class LogSource : uint8_t {
      kEngine = 0,
      kClient,
    };

    void logMessage(LogSource log_source, Level log_level, std::string_view format_string, fmt::format_args args);
  }

#ifdef RP_ENGINE
  template<typename... Args>
  void rp_trace(std::string_view format, const Args&... args) {
    internal::logMessage(internal::LogSource::kEngine, Level::kTrace, format, fmt::make_format_args(args...));
  }

  template<typename... Args>
  void rp_info(std::string_view format, const Args&... args) {
    internal::logMessage(internal::LogSource::kEngine, Level::kInfo, format, fmt::make_format_args(args...));
  }

  template<typename... Args>
  void rp_warn(std::string_view format, const Args&... args) {
    internal::logMessage(internal::LogSource::kEngine, Level::kWarn, format, fmt::make_format_args(args...));
  }

  template<typename... Args>
  void rp_error(std::string_view format, const Args&... args) {
    internal::logMessage(internal::LogSource::kEngine, Level::kError, format, fmt::make_format_args(args...));
  }
#else
  template<typename... Args>
  void trace(std::string_view format, const Args&... args) {
    internal::logMessage(internal::LogSource::kClient, Level::kTrace, format, fmt::make_format_args(args...));
  }

  template<typename... Args>
  void info(std::string_view format, const Args&... args) {
    internal::logMessage(internal::LogSource::kClient, Level::kInfo, format, fmt::make_format_args(args...));
  }

  template<typename... Args>
  void warn(std::string_view format, const Args&... args) {
    internal::logMessage(internal::LogSource::kClient, Level::kWarn, format, fmt::make_format_args(args...));
  }

  template<typename... Args>
  void error(std::string_view format, const Args&... args) {
    internal::logMessage(internal::LogSource::kClient, Level::kError, format, fmt::make_format_args(args...));
  }
#endif
}