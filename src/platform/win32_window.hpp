#pragma once

#include "core/window.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace rp {
  class Win32Window : public rp::Window {
    public:
      Win32Window(std::string_view title, uint32_t width, uint32_t height);
      ~Win32Window();

      bool processMessages();
      void setCallback(const std::function<void()>& callback);

    protected:
      static const char* windowClassName;   // name for window class, needed for createWindow(...) call
      static bool wcRegistered;             // ensures window class is only registered once

      HWND mHandle = NULL;

      LRESULT internalWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
      static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
  };
}