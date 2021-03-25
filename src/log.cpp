#include "rp/log.hpp"
#include "rp/color.hpp"

#include <fmt/format.h>

namespace rp::log {

  std::string    app_prefix    = "Client";
  constexpr auto rapier_prefix = "Rapier";
  
  constexpr std::array<const char*, static_cast<size_t>(Level::kSize)> kLevelPrefixes {
    "Trace:",
    " Info:",
    " Warn:",
    "Error:"
  };

  constexpr std::array<rp::Color, static_cast<size_t>(log::Level::kSize)> kLevelColors = {
    Color{0xCC, 0xCC, 0xCC, 0xFF},
    Color{0xFF, 0xFF, 0x00, 0xFF},
    Color{0xFF, 0x99, 0x00, 0xFF},
    Color{0xFF, 0x00, 0x00, 0xFF}
  };

  namespace internal { 
    void logMessage(LogSource log_source, Level log_level, std::string_view format_string, fmt::format_args args) {
      auto setColor = [](rp::Color color) {
        fmt::print("\033[38;2;{};{};{}m", color.r, color.g, color.b);
      };

      auto resetColor = []() {
        fmt::print("\033[0m");
      };


      setColor(kLevelColors[static_cast<size_t>(log_level)]);

      // Log Format: [Source][Level] {Formatted String}
      fmt::vprint(
        fmt::format("[{}] {} {}\n",
          (log_source == LogSource::kEngine) ? rapier_prefix : app_prefix,
          kLevelPrefixes[static_cast<size_t>(log_level)],
          format_string),
        args);

      resetColor();

    }
  }
}