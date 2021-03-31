#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include "core/keyboard.hpp"

namespace rp::input {
  Keyboard::Key translateWin32KeyCode(WPARAM win32_key_code, LPARAM flags);
}