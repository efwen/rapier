#pragma once
#include <memory>

#include "app.hpp"
#include "core/window.hpp"

namespace rp {

  struct StartupProperties {
    std::string logClientPrefix;
    Window::Properties windowProperties;
  };

  void run(std::unique_ptr<App> app, StartupProperties startupProperties);
}