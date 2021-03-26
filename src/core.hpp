#pragma once
#include <memory>

#include "app.hpp"

namespace rp {
  void run(std::unique_ptr<App> app, int argc, char** argv);
}