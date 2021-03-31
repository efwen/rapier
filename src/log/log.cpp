#include "pch.hpp"

#include "util/util_types.hpp"
namespace rp::log {

  size_t calculateSourcePrefixLength(size_t client_prefix_length);

  std::string                client_prefix = "Client";
  const std::string          rapier_prefix = "Rapier";
  const size_t        rapier_prefix_length = rapier_prefix.length();
  size_t              source_prefix_length = calculateSourcePrefixLength(client_prefix.length()); 

  constexpr EnumMatchedArray<log::Level, const char*> kLevelPrefixes {
    "Trace:",
    " Info:",
    " Warn:",
    "Error:"
  };

  constexpr EnumMatchedArray<log::Level, fmt::color> kLevelColors {
    fmt::color::slate_gray,
    fmt::color::yellow,
    fmt::color::orange_red,
    fmt::color::red
  };

  void logMessage(Source log_source, Level log_level, std::string_view format_string, fmt::format_args args) {
    // Log Format: [Source] Level: {Formatted String}
    auto source_prefix = fmt::format("{:<{}}",
      fmt::format("[{}]", ((log_source == Source::Engine) ? rapier_prefix : client_prefix)),
      source_prefix_length); 

    auto message = fmt::vformat(format_string, args);  

    fmt::print(fg(kLevelColors[static_cast<size_t>(log_level)]),
      fmt::format("{} {} {}\n",
        source_prefix,
        kLevelPrefixes[static_cast<size_t>(log_level)],
        message));
  }

  void logEngineMessage(Level log_level, std::string_view format_string, fmt::format_args args) {
    logMessage(Source::Engine, log_level, format_string, args);
  }

  void logClientMessage(Level log_level, std::string_view format_string, fmt::format_args args) {
    logMessage(Source::Client, log_level, format_string, args);
  }

  size_t calculateSourcePrefixLength(size_t client_prefix_length) {
    constexpr size_t    base_prefix_spacing = 2;
    return std::max(rapier_prefix.length(), client_prefix_length) + base_prefix_spacing;
  }

  void setClientPrefix(std::string_view prefix) {
    client_prefix = prefix; 
    source_prefix_length = calculateSourcePrefixLength(client_prefix.length());
  }

}