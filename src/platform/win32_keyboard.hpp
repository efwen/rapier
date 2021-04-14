#pragma once

//include windows type definitions
#include <WinDef.h>

#include "input/keyboard.hpp"

namespace rp::input {
  Keyboard::Key translateWin32KeyCode(WPARAM win32_key_code, LPARAM flags);
}