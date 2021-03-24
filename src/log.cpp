#include "rp/log.hpp"
#include <fmt/format.h>

namespace rp::log {


  std::string    app_prefix = "[App]";
  constexpr auto rapier_prefix = "[Rapier]";

  void init() {

  }
  namespace internal { 
    void logMessage(LogSource log_source, Level log_level, std::string_view format_string, fmt::format_args args) {
      fmt::vprint(
        fmt::format("{}[{}] {}\n",
        (log_source == LogSource::kEngine) ? rapier_prefix : app_prefix,
        levelNames[static_cast<size_t>(log_level)],
        format_string),
      args);
    }
  }
}