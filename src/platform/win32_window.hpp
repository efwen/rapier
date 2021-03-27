#pragma once

#include "core/window.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace rp {
  class Win32Window : public rp::Window {
    public:
      Win32Window(std::string_view title, uint32_t width, uint32_t height);
      ~Win32Window();

      void setCallback(const std::function<void()>& callback);
  };
}