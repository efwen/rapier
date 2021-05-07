#include "core/event.hpp"

namespace rp {
    const std::array<const char*, INDEX_CAST(Event::Type::ENUM_SIZE)> Event::eventNames = {
        "Invalid",
        "WindowClosed",
        "KeyPressed",
        "KeyReleased",
        "MouseButtonPressed",
        "MouseButtonReleased",
        "MouseWheelScrolled",
        "MouseMoved"
    };
}
