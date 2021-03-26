#include "pch.hpp"

#include "util/version.hpp"

namespace rp {
  constexpr Version version{0, 0, 1};

  std::string Version::toString() const {
    return fmt::format("{}.{}.{}", major, minor, patch);
  }

  Version getVersion() {
    return version;
  }
}