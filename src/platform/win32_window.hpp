#pragma once

#include "core/window.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>

namespace rp {
  class Win32Window : public rp::Window {
    public:
      Win32Window(const Properties& props);
      ~Win32Window();

      bool processMessages();

    protected:
      static const char* windowClassName;   // name for window class, needed for createWindow(...) call
      static bool wcRegistered;             // ensures window class is only registered once

      HWND mHandle = NULL;

      LRESULT internalWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
      static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
  };
}