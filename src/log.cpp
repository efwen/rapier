#include "log.hpp"
#include "rp/color.hpp"

#include <fmt/format.h>

namespace rp::log {

  size_t calculateSourcePrefixLength(size_t client_prefix_length);

  std::string                   client_prefix = "Client";
  const std::string             rapier_prefix = "Rapier";
  const size_t           rapier_prefix_length = rapier_prefix.length();

  size_t              source_prefix_length = calculateSourcePrefixLength(client_prefix.length()); 

  size_t calculateSourcePrefixLength(size_t client_prefix_length) {
    constexpr size_t    base_prefix_spacing = 2;
    return std::max(rapier_prefix.length(), client_prefix_length) + base_prefix_spacing;
  }

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

  void logMessage(Source log_source, Level log_level, std::string_view format_string, fmt::format_args args) {
    auto setColor = [](rp::Color color) {
      fmt::print("\033[38;2;{};{};{}m", color.r, color.g, color.b);
    };

    auto resetColor = []() {
      fmt::print("\033[0m");
    };


    setColor(kLevelColors[static_cast<size_t>(log_level)]);

    // Log Format: [Source][Level] {Formatted String}
    auto source_prefix = fmt::format("{:<{}}",
      fmt::format("[{}]", ((log_source == Source::kEngine) ? rapier_prefix : client_prefix)),
      source_prefix_length); 
    fmt::vprint(fmt::format("{} {} {}\n", source_prefix, kLevelPrefixes[static_cast<size_t>(log_level)], format_string), args);

    resetColor();
  }

  void logEngineMessage(Level log_level, std::string_view format_string, fmt::format_args args) {
    logMessage(Source::kEngine, log_level, format_string, args);
  }

  void logClientMessage(Level log_level, std::string_view format_string, fmt::format_args args) {
    logMessage(Source::kClient, log_level, format_string, args);
  }

  void setClientPrefix(std::string_view prefix) {
    client_prefix = prefix; 
    source_prefix_length = calculateSourcePrefixLength(client_prefix.length());
  }

}