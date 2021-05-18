#include "pch.hpp"
#include "platform/win32_window.hpp"

#include "input/keyboard.hpp"
#include "input/mouse.hpp"

#include "platform/win32_keyboard.hpp"
namespace rp {

  Win32Window::Win32Window(const Properties& props) : Window(props) {
    static const char* windowClassName = "RPWindowClass";
    static bool wcRegistered = false;

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
      props.title.data(),
      WS_OVERLAPPEDWINDOW,
      300, 50,
      props.width, props.height,
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

  void* Win32Window::getHandle() {
      return (void*)mHandle;
  }

  bool Win32Window::isMinimized() {
      return mProps.isMinimized;
  }

  LRESULT Win32Window::internalWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    auto getMouseButtonId = [=]() { return static_cast<input::Mouse::Button>((message - WM_MOUSEFIRST) / 3); };
    auto getMousePos = [=]() { return input::Mouse::Position{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)}; };

    switch(message) {
      case WM_CLOSE:
      {
        Event event;
        event.type = Event::Type::WindowClosed;
        if(mCallback) mCallback(event);
        PostQuitMessage(0);
        return 0;
      }
      case WM_SIZE:
      {
          switch (wParam) {
          case SIZE_MINIMIZED:
          {
              mProps.isMinimized = true;
              break;
          }
          case SIZE_MAXIMIZED:
          {
              mProps.isMinimized = false;
              break;
          }
          case SIZE_RESTORED:
          {
              mProps.isMinimized = false;
              break;
          }
        }
        return 0;
      }
      case WM_SIZING:
      {
          RECT* rect = (RECT*)lParam;
          mProps.width = rect->right - rect->left;
          mProps.height = rect->bottom - rect->top;
          return 0;
      }
      case WM_KEYDOWN:
      {
        Event event;
        event.type = Event::Type::KeyPressed;
        event.key_code = input::translateWin32KeyCode(wParam, lParam);
        if(mCallback) mCallback(event);
        return 0;
      }
      case WM_KEYUP:
      {
        Event event;
        event.type = Event::Type::KeyReleased;
        event.key_code = input::translateWin32KeyCode(wParam, lParam);
        if(mCallback) mCallback(event);
        return 0;
      }
      case WM_SYSKEYDOWN:
      {
        Event event;
        event.type = Event::Type::KeyPressed;
        event.key_code = input::translateWin32KeyCode(wParam, lParam);
        if(mCallback) mCallback(event);
        return 0;
      }
      case WM_SYSKEYUP:
      {
        Event event;
        event.type = Event::Type::KeyReleased;
        event.key_code = input::translateWin32KeyCode(wParam, lParam);
        if(mCallback) mCallback(event);
        return 0;
      }
      case WM_LBUTTONDOWN:
      case WM_RBUTTONDOWN:
      case WM_MBUTTONDOWN:
      {
        Event event;
        event.type = Event::Type::MouseButtonPressed;
        event.mouse.button = getMouseButtonId();
        event.mouse.position = getMousePos();
        if(mCallback) mCallback(event);
        return 0;
      }
      case WM_LBUTTONUP:
      case WM_RBUTTONUP:
      case WM_MBUTTONUP:
      {
        Event event;
        event.type = Event::Type::MouseButtonReleased;
        event.mouse.button = getMouseButtonId();
        event.mouse.position = getMousePos(); 
        if(mCallback) mCallback(event);
        return 0;
      }
      case WM_MOUSEMOVE:
      {
        Event event;
        event.type = Event::Type::MouseMoved;
        event.mouse.position = getMousePos();
        if(mCallback) mCallback(event);
        return 0;
      }
      case WM_MOUSEWHEEL:
      {
        Event event;
        event.type = Event::Type::MouseWheelScrolled;
        event.mouse.position = getMousePos(); 
        event.mouse.scroll = GET_WHEEL_DELTA_WPARAM(wParam);
        if(mCallback) mCallback(event);
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