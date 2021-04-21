#pragma once

#include "core/event.hpp"

namespace rp {
  class App {
  public:
    virtual ~App() = default;  
    virtual void init() = 0;
    virtual void onEvent(const rp::Event& e) = 0;
    virtual void update() = 0;
    virtual void shutdown() = 0;
  };
}