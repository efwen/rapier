#include "pch.hpp"

#include "platform/win32_window.hpp"

namespace rp {
  Win32Window::Win32Window(std::string_view title, uint32_t width, uint32_t height) {
    log::info("Creating Win32 Window!");
    log::info("Title: {}, Resolution: {}x{} pixels", title, width, height);
  }

  Win32Window::~Win32Window() {
    log::info("Destroying Win32 Window!");
  } 

  void Win32Window::setCallback(const std::function<void()>& callback) {
    log::error("Win32Window setCallback() Function not implemented!");
  }
}