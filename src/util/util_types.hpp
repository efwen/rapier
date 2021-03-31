#pragma once

#include <type_traits>
#include <array>

namespace rp {
  template<typename E>
  concept enum_type = std::is_enum_v<E>;

  //A std::array matching the length of a given enum  
  //Assumes E is a contiguous enum starting from 0 and ending with a value named ENUM_SIZE
  template<enum_type E, typename ArrayType>
  using EnumMatchedArray = std::array<ArrayType, static_cast<size_t>(E::ENUM_SIZE)>;
}