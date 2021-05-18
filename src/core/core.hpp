#pragma once
#include <memory>
#include <string>

#include "app.hpp"
#include "core/window.hpp"
#include "util/version.hpp"

namespace rp {

  struct StartupProperties {
    std::string logClientPrefix;
    Window::Properties windowProperties;
    std::string appName = "App";
    util::Version appVersion = { 0, 0, 1 };
  };

  void run(std::unique_ptr<App> app, StartupProperties startupProperties);
}