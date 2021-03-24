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

  constexpr std::array<const char*, static_cast<size_t>(Level::kSize)> levelNames {
    "kTrace",
    "kInfo",
    "kWarn",
    "kError"
  };

  void init();

  namespace internal {
    enum class LogSource : uint8_t {
      kEngine = 0,
      kClient,
    };

    void logMessage(LogSource log_source, Level log_level, std::string_view format_string, fmt::format_args args);
  }

  template<Level L, typename... Args>
  void write(std::string_view format, const Args&... args)  {
#ifdef RP_ENGINE
    internal::logMessage(internal::LogSource::kEngine, L, format, fmt::make_format_args(args...));
#else
    internal::logMessage(internal::LogSource::kClient, L, format, fmt::make_format_args(args...));
#endif
  }
}