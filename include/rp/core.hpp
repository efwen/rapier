#pragma once
#include <memory>

#include "rp/app.hpp"

namespace rp {
  void run(std::unique_ptr<App> app, int argc, char** argv);
}