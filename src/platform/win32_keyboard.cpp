#include "pch.hpp"
#include "platform/win32_keyboard.hpp"

namespace rp::input {
  Keyboard::Key translateWin32KeyCode(WPARAM win32_key_code, LPARAM flags) {

    switch (win32_key_code) {
      case 'A':
        return Keyboard::Key::A;
      case 'B':
        return Keyboard::Key::B;
      case 'C':
        return Keyboard::Key::C;
      case 'D':
        return Keyboard::Key::D;
      case 'E':
        return Keyboard::Key::E;
      case 'F':
        return Keyboard::Key::F;
      case 'G':
        return Keyboard::Key::G;
      case 'H':
        return Keyboard::Key::H;
      case 'I':
        return Keyboard::Key::I;
      case 'J':
        return Keyboard::Key::J;
      case 'K':
        return Keyboard::Key::K;
      case 'L':
        return Keyboard::Key::L;
      case 'M':
        return Keyboard::Key::M;
      case 'N':
        return Keyboard::Key::N;
      case 'O':
        return Keyboard::Key::O;
      case 'P':
        return Keyboard::Key::P;
      case 'Q':
        return Keyboard::Key::Q;
      case 'R':
        return Keyboard::Key::R;
      case 'S':
        return Keyboard::Key::S;
      case 'T':
        return Keyboard::Key::T;
      case 'U':
        return Keyboard::Key::U;
      case 'V':
        return Keyboard::Key::V;
      case 'W':
        return Keyboard::Key::W;
      case 'X':
        return Keyboard::Key::X;
      case 'Y':
        return Keyboard::Key::Y;
      case 'Z':
        return Keyboard::Key::Z;
      
      case VK_SPACE:
        return Keyboard::Key::Space;
      case VK_OEM_COMMA:
        return Keyboard::Key::Comma;
      case VK_OEM_PERIOD:
        return Keyboard::Key::Period;
      case VK_OEM_1:
        return Keyboard::Key::Semicolon;
      case VK_OEM_2:
        return Keyboard::Key::Slash;
      case VK_OEM_3:
        return Keyboard::Key::Tilde;
      case VK_OEM_4:
        return Keyboard::Key::LeftBracket;
      case VK_OEM_6:
        return Keyboard::Key::RightBracket;
      case VK_OEM_5:
        return Keyboard::Key::Backslash;
      case VK_OEM_7:
        return Keyboard::Key::Quote;
      
      case VK_SHIFT:
      {
        //VK_SHIFT doesn't differentiate between left and right shift, but the actual scan code is in the flags.
        //MapVirtualKey gives us the scan code of left shift. We then fetch bits containing the current scan code
        //  (bits 16-23) and compare to see if its left or right shift
        //ref: https://docs.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input#keystroke-message-flags
        static UINT left_shift_scan_code = MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC);
        UINT scan_code = (flags & 0x00FF0000) >> 16;
        return (scan_code == left_shift_scan_code) ? Keyboard::Key::LeftShift : Keyboard::Key::RightShift;
      }
      case VK_CONTROL:
        return (HIWORD(flags) & KF_EXTENDED) ? Keyboard::Key::RightCtrl : Keyboard::Key::LeftCtrl;
      case VK_MENU:
        return (HIWORD(flags) & KF_EXTENDED) ? Keyboard::Key::RightAlt : Keyboard::Key::LeftAlt;
      case VK_LWIN:
        return Keyboard::Key::LeftSystem;
      case VK_RWIN:
        return Keyboard::Key::RightSystem;
      case VK_TAB:
        return Keyboard::Key::Tab;
      case VK_BACK:
        return Keyboard::Key::Backspace;
      case VK_RETURN:
        return Keyboard::Key::Enter;
      case VK_CAPITAL:
        return Keyboard::Key::CapsLock;
      case VK_ESCAPE:
        return Keyboard::Key::Escape;
      
      case '0':
        return Keyboard::Key::Num0;
      case '1':
        return Keyboard::Key::Num1;
      case '2':
        return Keyboard::Key::Num2;
      case '3':
        return Keyboard::Key::Num3;
      case '4':
        return Keyboard::Key::Num4;
      case '5':
        return Keyboard::Key::Num5;
      case '6':
        return Keyboard::Key::Num6;
      case '7':
        return Keyboard::Key::Num7;
      case '8':
        return Keyboard::Key::Num8;
      case '9':
        return Keyboard::Key::Num9;
      case VK_OEM_MINUS:
        return Keyboard::Key::Dash;
      case VK_OEM_PLUS:
        return Keyboard::Key::Equals;
      
      case VK_NUMPAD0:
        return Keyboard::Key::Numpad0;
      case VK_NUMPAD1:
        return Keyboard::Key::Numpad1;
      case VK_NUMPAD2:
        return Keyboard::Key::Numpad2;
      case VK_NUMPAD3:
        return Keyboard::Key::Numpad3;
      case VK_NUMPAD4:
        return Keyboard::Key::Numpad4;
      case VK_NUMPAD5:
        return Keyboard::Key::Numpad5;
      case VK_NUMPAD6:
        return Keyboard::Key::Numpad6;
      case VK_NUMPAD7:
        return Keyboard::Key::Numpad7;
      case VK_NUMPAD8:
        return Keyboard::Key::Numpad8;
      case VK_NUMPAD9:
        return Keyboard::Key::Numpad9;
      
      case VK_ADD:
        return Keyboard::Key::Add;
      case VK_SUBTRACT:
        return Keyboard::Key::Subtract;
      case VK_MULTIPLY:
        return Keyboard::Key::Multiply;
      case VK_DIVIDE:
        return Keyboard::Key::Divide;
      case VK_NUMLOCK:
        return Keyboard::Key::NumLock;

      case VK_F1:
        return Keyboard::Key::F1;
      case VK_F2:
        return Keyboard::Key::F2;
      case VK_F3:
        return Keyboard::Key::F3;
      case VK_F4:
        return Keyboard::Key::F4;
      case VK_F5:
        return Keyboard::Key::F5;
      case VK_F6:
        return Keyboard::Key::F6;
      case VK_F7:
        return Keyboard::Key::F7;
      case VK_F8:
        return Keyboard::Key::F8;
      case VK_F9:
        return Keyboard::Key::F9;
      case VK_F10:
        return Keyboard::Key::F10;
      case VK_F11:
        return Keyboard::Key::F11;
      case VK_F12:
        return Keyboard::Key::F12;

      case VK_SNAPSHOT:
        return Keyboard::Key::PrintScreen;
      case VK_SCROLL:
        return Keyboard::Key::ScrollLock;
      case VK_PAUSE:
        return Keyboard::Key::Pause;


      case VK_INSERT:
        return Keyboard::Key::Insert;
      case VK_DELETE:
        return Keyboard::Key::Delete;
      case VK_HOME:
        return Keyboard::Key::Home;
      case VK_END:
        return Keyboard::Key::End;
      case VK_PRIOR:
        return Keyboard::Key::PageUp;
      case VK_NEXT:
        return Keyboard::Key::PageDown;

      case VK_UP:
        return Keyboard::Key::Up;
      case VK_DOWN:
        return Keyboard::Key::Down;
      case VK_LEFT:
        return Keyboard::Key::Left;
      case VK_RIGHT:
        return Keyboard::Key::Right;
    }

    log::rp_error("KeyCode {:#x} Not Handled!", win32_key_code);
    return Keyboard::Key::Invalid;
  }
}