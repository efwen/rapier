#pragma once

#include <string_view>
#include <fmt/format.h>

namespace rp::log {
  enum class Level : uint8_t {
    kTrace = 0,
    kInfo,
    kWarn,
    kError,
    kSize,
  };

  void init();

  namespace internal {
    void logCoreMessage(Level log_level, std::string_view format_string, fmt::format_args args);
    void logClientMessage(Level log_level, std::string_view format_string, fmt::format_args args);
  }

  template<Level L, typename... Args>
  void core(std::string_view format, const Args&... args)  {
    internal::logCoreMessage(L, format, fmt::make_format_args(args...));
  }

  template<Level L, typename... Args>
  void client(std::string_view forrmat, const Args&... args) {
    internal::logClientMessage(L, format, fmt::make_format_args(args...));
  }
}