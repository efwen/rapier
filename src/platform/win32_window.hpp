#pragma once

#include "core/window.hpp"

//include windows type definitions
#include <winDef.h>

namespace rp {
  class Win32Window : public rp::Window {
    public:
      Win32Window(const Properties& props);
      ~Win32Window();

      bool processMessages();

      void* getHandle();
      bool isMinimized();

    protected:
      HWND mHandle = NULL;

      LRESULT internalWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
      static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
  };
}