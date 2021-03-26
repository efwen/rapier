#pragma once

//standard library headers
#include <cstdint>
#include <array>
#include <memory>

//external libaries
#ifdef WIN32
  #define WIN32_LEAN_AND_MEAN
  #include <Windows.h>
#endif

#include <fmt/format.h>
#include <fmt/color.h>

//common internal headers
#include "log/log_internal.hpp"