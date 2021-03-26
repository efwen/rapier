#pragma once

namespace rp {
  class App {
  public:
    virtual ~App() = default;  
    virtual void init() = 0;
    virtual void update() = 0;
    virtual void shutdown() = 0;
  };
}