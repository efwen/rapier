#pragma once
#include <string>

namespace rp {
  struct Version{
    uint16_t major;
    uint16_t minor;
    uint16_t patch;
    std::string toString() const;
  };

  Version getVersion();
}
