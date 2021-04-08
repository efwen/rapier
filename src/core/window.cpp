#include "pch.hpp"

#include "core/window.hpp"
#include "platform/win32_window.hpp"

namespace rp {

  Window::Window(const Window::Properties& props) : mProps(props) {
    log::rp_info("Creating Window!");
    log::rp_info("Title: {}, Resolution: {}x{} pixels", props.title, props.width, props.height);
  }
  std::unique_ptr<Window> createWindow(const Window::Properties& props) {
    return std::make_unique<Win32Window>(props);
  }
}