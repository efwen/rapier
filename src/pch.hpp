#pragma once

//standard library headers
#include <algorithm>
#include <array>
#include <cstdint>
#include <exception>
#include <iomanip>
#include <memory>
#include <random>
#include <sstream>
#include <optional>

//fmt format library
#include <fmt/format.h>
#include <fmt/color.h>

//vulkan rendering api
#include <vulkan/vulkan.h>

//Windows headers
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>

//common internal headers
#include "log/log_internal.hpp"