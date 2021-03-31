#pragma once

#include "util/util_types.hpp"
#include "input/mouse.hpp"
#include "input/keyboard.hpp"

namespace rp {
  struct Event {
    enum class Type {
      Invalid,
      WindowClosed,
      KeyPressed,
      KeyReleased,
      MouseButtonPressed,
      MouseButtonReleased,
      MouseWheelScrolled,
      MouseMoved,
      ENUM_SIZE,
    };

    struct MouseInfo {
      input::Mouse::Button button;
      input::Mouse::Position position;
      int scroll;
    };

    Type type;
    input::Keyboard::Key key_code{};
    MouseInfo mouse{};

    static constexpr const char* GetEventName(Event::Type event_type) {
      return eventNames[static_cast<size_t>(event_type)];
    }

private:
    static constexpr EnumMatchedArray<Event::Type, const char*> eventNames {
      "Invalid",
      "WindowClosed",
      "KeyPressed",
      "KeyReleased",
      "MouseButtonPressed",
      "MouseButtonReleased",
      "MouseWheelScrolled",
      "MouseMoved"
    };
  };
}