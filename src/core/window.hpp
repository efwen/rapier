#pragma once

#include <functional>
#include <memory>

#include "event.hpp"
namespace rp {
  class Window {
    public:
      using Callback = std::function<void(const Event& e)>;
      struct Properties {
        std::string title;
        uint32_t width = 0;
        uint32_t height = 0;
        bool isMinimized = false;
      };

      Window(const Properties& props);
      virtual ~Window() = default;

      virtual bool processMessages() = 0;
      virtual void setCallback(const Callback& callback) {
        mCallback = callback;
      };

      virtual void* getHandle() = 0;
      virtual bool isMinimized() = 0;

      virtual Properties GetProperties();

    protected:
      Callback mCallback;
      Properties mProps;
  };

  std::unique_ptr<Window> createWindow(const Window::Properties& props);
}