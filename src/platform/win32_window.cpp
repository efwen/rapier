#include "pch.hpp"

#include "platform/win32_window.hpp"

#include <exception>
namespace rp {
  const char* Win32Window::windowClassName = "RPWindowClass";
  bool Win32Window::wcRegistered = false;

  Win32Window::Win32Window(std::string_view title, uint32_t width, uint32_t height) {
    log::rp_info("Creating Win32 Window!");
    log::rp_info("Title: {}, Resolution: {}x{} pixels", title, width, height);

    if(!wcRegistered) {
      WNDCLASSEX wc = {};
      wc.cbSize = sizeof(wc);
      wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
      wc.lpfnWndProc = WndProc;
      wc.hInstance = GetModuleHandle(NULL);
      wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
      wc.lpszClassName = windowClassName;
      wcRegistered = RegisterClassEx(&wc);
      if(!wcRegistered) {
        throw std::exception(fmt::format("Failed to register window class! GetLastError = 0x{:x}", GetLastError()).c_str());
      }
    }

    mHandle = CreateWindowEx(
      0,
      windowClassName,
      title.data(),
      WS_OVERLAPPEDWINDOW,
      300, 50,
      width, height,
      NULL,
      NULL,
      GetModuleHandle(NULL),
      this
    );

    if(mHandle == NULL) {
      throw std::exception(fmt::format("Failed to create Window! GetLastError = 0x{:x}", GetLastError()).c_str());
    };

    ShowWindow(mHandle, SW_SHOW);
  }

  Win32Window::~Win32Window() {
    log::rp_info("Destroying Win32 Window!");
    DestroyWindow(mHandle);
  } 

  bool Win32Window::processMessages(){
    MSG msg = {};
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      if(msg.message == WM_QUIT) {
        return false;
      }

      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    return true;
  }

  void Win32Window::setCallback(const std::function<void()>& callback) {
    log::rp_error("Win32Window setCallback() Function not implemented!");
  }

  LRESULT Win32Window::internalWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch(message) {
      case WM_CLOSE:
      {
        PostQuitMessage(0);
        return 0;
      }
      default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
  }

  LRESULT CALLBACK Win32Window::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    Win32Window* window = nullptr;
    if(message == WM_NCCREATE) {
      //stores a pointer to the associated Win32Window object in the GWLP_USERDATA attribute
      //this allows us to call the internal version of WndProc in future messages
      CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lParam);
      window = static_cast<Win32Window*>(create_struct->lpCreateParams);
      window->mHandle = hwnd;
      SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
    } else {
      window = reinterpret_cast<Win32Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if(window) {
      return window->internalWndProc(hwnd, message, wParam, lParam);
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
  }

}