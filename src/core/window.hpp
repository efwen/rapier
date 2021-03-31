#pragma once

#include <functional>
#include <memory>

#include "event.hpp"
namespace rp {
  class Window {
    public:
      using Callback = std::function<void(const Event& e)>;

      virtual ~Window() = default;

      virtual bool processMessages() = 0;
      virtual void setCallback(const Callback& callback) {
        mCallback = callback;
      };

    protected:
      Callback mCallback;
  };

  std::unique_ptr<Window> createWindow(std::string_view title, uint32_t width, uint32_t height);
}