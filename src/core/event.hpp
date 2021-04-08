#pragma once

#include <string>
#include <fmt/format.h>

#include "util/util.hpp"
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
      input::Mouse::Position position;
      input::Mouse::Button button;
      int scroll;
    };

    Type type;
    input::Keyboard::Key key_code{};
    MouseInfo mouse{};

    static constexpr auto GetEventName(Event::Type event_type) {
      return eventNames[INDEX_CAST(event_type)];
    }

private:
    static constexpr auto eventNames = std::to_array({
      "Invalid",
      "WindowClosed",
      "KeyPressed",
      "KeyReleased",
      "MouseButtonPressed",
      "MouseButtonReleased",
      "MouseWheelScrolled",
      "MouseMoved"
    });
    static_assert(eventNames.size() == INDEX_CAST(Event::Type::ENUM_SIZE));
  };
}

template <>
struct fmt::formatter<rp::Event> {
  constexpr auto parse(fmt::format_parse_context& ctx) {
    auto it = ctx.begin(), end = ctx.end();
    if(it != end && *it != '}')
      throw format_error("invalid format parsing rp::Event");
    return it;
  }

  template<typename FormatContext>
  auto format(const rp::Event& e, FormatContext& ctx) {
    std::string_view fmt_string;
    switch(e.type) {
      case rp::Event::Type::Invalid:
      case rp::Event::Type::WindowClosed:
        fmt_string = "<{0}>";
        break;
      case rp::Event::Type::KeyPressed:
      case rp::Event::Type::KeyReleased:
        fmt_string = "<{0} ({1})>";
        break;
      case rp::Event::Type::MouseButtonPressed:
      case rp::Event::Type::MouseButtonReleased:
        fmt_string = "<{0} (Button {2})>";
        break;
      case rp::Event::Type::MouseMoved:
        fmt_string = "<{0} ({3}, {4})>";
        break;
      case rp::Event::Type::MouseWheelScrolled:
        fmt_string = "<{0} ({5})>";
        break;
      case rp::Event::Type::ENUM_SIZE:
        throw format_error("Invalid use of ENUM_SIZE as Event Type!");
        break;
    };

    if(fmt_string.empty())
      throw format_error("Event::Type not handled!");

    return format_to(
      ctx.out(),
      fmt_string,
      rp::Event::GetEventName(e.type),
      rp::input::Keyboard::GetKeyName(e.key_code),
      e.mouse.button,
      e.mouse.position.x,
      e.mouse.position.y,
      e.mouse.scroll);
  } 
};