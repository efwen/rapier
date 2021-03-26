#pragma once

#include <functional>
#include <memory>

namespace rp {
  class Window {
    public:
      virtual ~Window() = default;

      virtual void setCallback(const std::function<void()>& callback) = 0;
  };

  std::unique_ptr<Window> createWindow(std::string_view title, uint32_t width, uint32_t height);
}