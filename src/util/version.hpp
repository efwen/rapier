#pragma once
#include <string>

namespace rp::util {
  struct Version{
    uint8_t major;
    uint8_t minor;
    uint8_t patch;
    std::string toString() const;
  };
  constexpr Version rapierVersion{0, 0, 1};
}
