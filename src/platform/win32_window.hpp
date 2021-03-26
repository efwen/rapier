#pragma once

#include "core/window.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace rp {
  class Win32Window : public rp::Window {
    public:
      Win32Window(std::string_view title, uint32_t width, uint32_t height) {
        log::info("Creating Win32 Window!");
        log::info("Title: {}, Resolution: {}x{} pixels", title, width, height);
      }

      ~Win32Window() {
        log::info("Destroying Win32 Window!");
      } 

      void setCallback(const std::function<void()>& callback) {
        log::error("Win32Window setCallback() Function not implemented!");
      }
  };
}