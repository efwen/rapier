#include "pch.hpp"
#include "util/version.hpp"

namespace rp::util {
  std::string Version::toString() const {
    return fmt::format("{}.{}.{}", major, minor, patch);
  }
}