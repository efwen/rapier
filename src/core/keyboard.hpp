#pragma once

#include "util/util_types.hpp"

namespace rp::input {
  struct Keyboard {
    enum class Key : uint16_t {
      Invalid,
      A, B, C, D, E, F, G, H, I, J, K, L, M,
      N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
      
      Space, Comma, Period, Semicolon, Slash, Tilde,
      LeftBracket, RightBracket, Backslash, Quote,

      LeftShift, RightShift, LeftCtrl, RightCtrl,
      LeftAlt, RightAlt, LeftSystem, RightSystem,
      Tab, Backspace, Enter, CapsLock, Escape,

      Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9, Dash, Equals,

      Numpad0, Numpad1, Numpad2, Numpad3, Numpad4, Numpad5, Numpad6,
      Numpad7, Numpad8, Numpad9, Add, Subtract, Multiply, Divide, NumLock,

      F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

      PrintScreen, ScrollLock, Pause,
      Insert, Delete, Home, End, PageUp, PageDown,
      Up, Down, Left, Right,

      ENUM_SIZE,
    };

    static const char* GetKeyName(Keyboard::Key key) {
      return keyNames[static_cast<size_t>(key)];
    }

    private:
      static constexpr EnumMatchedArray<Key, const char*> keyNames {
        "Invalid",
        "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
        "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",

        "Space", "Comma", "Period", "Semicolon", "Slash", "Tilde",
        "LeftBracket", "RightBracket", "Backslash", "Quote",

        "LeftShift", "RightShift", "LeftCtrl", "RightCtrl",
        "LeftAlt", "RightAlt", "LeftSystem", "RightSystem",
        "Tab", "Backspace", "Enter", "CapsLock", "Escape",

        "Num0", "Num1", "Num2", "Num3", "Num4", "Num5", "Num6", "Num7", "Num8", "Num9", "Dash", "Equals",

        "Numpad0", "Numpad1", "Numpad2", "Numpad3", "Numpad4", "Numpad5", "Numpad6",
        "Numpad7", "Numpad8", "Numpad9", "Add", "Subtract", "Multiply", "Divide", "NumLock", 

        "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",

        "PrintScreen", "ScrollLock", "Pause",
        "Insert", "Delete", "Home", "End", "PageUp", "PageDown",
        "Up", "Down", "Left", "Right",
      };
  };
}