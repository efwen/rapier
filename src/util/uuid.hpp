#pragma once

#include <cstdint>
#include <array>
#include <string>

namespace rp {
  //Universally Unique Identifier (UUID)
  //Based on Version 4, Variant 1
  class UUID {
  public:
    UUID();
    UUID(const std::array<uint8_t, 16>& data);
    UUID(const std::array<uint8_t, 16>&& data);

    bool operator==(const UUID& rhs) const {
      return mData == rhs.mData;
    }
    bool operator!=(const UUID& rhs) const {
      return !(*this == rhs);
    }

    [[nodiscard]] static UUID Generate();

    //UUID string format: xxxxxxxx-xxxx-Mxxx-Nxxx-xxxxxxxxxxxx
    [[nodiscard]] std::string to_string() const noexcept;

    static const UUID nil_uuid;
  private:
    std::array<uint8_t, 16> mData;
  };
}