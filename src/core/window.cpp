#include "pch.hpp"

#include "core/window.hpp"
#include "platform/win32_window.hpp"

namespace rp {
  std::unique_ptr<Window> createWindow(std::string_view title, uint32_t width, uint32_t height) {
    return std::make_unique<Win32Window>(title, width, height);
  }
}