#include "pch.hpp"
#include "util/uuid.hpp"

namespace rp {
  constexpr std::array<uint8_t, 16> nil_uuid_data = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  const UUID UUID::nil_uuid = UUID(std::move(nil_uuid_data));

  UUID::UUID() : UUID(nil_uuid_data) {};
  UUID::UUID(const std::array<uint8_t, 16>& data) : mData(data) {}
  UUID::UUID(const std::array<uint8_t, 16>&& data) : mData(std::move(data)) {}

  std::string UUID::to_string() const noexcept {
    std::ostringstream ss;
    ss << std::hex << std::setfill('0');

    for (size_t i = 0; i < 16; i++) {
      if (i == 4 || i == 6 || i == 8 || i == 10) {
        ss << '-';
      }
      ss << std::setw(2) << static_cast<int>(mData[i]);
    }
    return ss.str();
  }

  UUID UUID::Generate() {
    static constexpr uint8_t version_4_code = 0x40;
    static constexpr uint8_t variant_1_code = 0x80;

    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());
    std::uniform_int_distribution<> distribution(0, 255);

    std::array<uint8_t, 16> data; 
    std::generate(data.begin(), data.end(), [&]() -> uint8_t { return distribution(generator); });

    data[6] = (data[6] & 0x0F) | version_4_code;
    data[8] = (data[8] & 0x3F) | variant_1_code;
    
    return UUID(std::move(data));
  }
}