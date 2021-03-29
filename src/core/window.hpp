#pragma once

#include <functional>
#include <memory>

namespace rp {
  class Window {
    public:
      using WindowCallback = std::function<void()>;
      virtual ~Window() = default;

      virtual bool processMessages() = 0;
      virtual void setCallback(const WindowCallback& callback) = 0;
    protected:
      WindowCallback mCallback;
  };

  std::unique_ptr<Window> createWindow(std::string_view title, uint32_t width, uint32_t height);
}