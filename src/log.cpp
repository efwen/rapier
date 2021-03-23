#include "rp/log.hpp"
#include <fmt/format.h>

namespace rp::log {
  namespace internal { 
    void logCoreMessage(Level log_level, std::string_view format_string, fmt::format_args args) {
      fmt::print("Hello from logCore\n");
    }

    void logClientMessage(Level log_level, std::string_view format_string, fmt::format_args args) {
      fmt::print("Hello from LogClient\n");
    }
  }
}