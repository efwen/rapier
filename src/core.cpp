#include <iostream>

#include "rp/core.hpp"
#include "rp/log.hpp"
#include "rp/app.hpp"
#include "rp/version.hpp"

namespace rp {
  void run(std::unique_ptr<App> app, int argc, char** argv) {
    log::write<log::Level::kInfo>("Rapier v{} started!", getVersion().toString());
    log::write<log::Level::kInfo>("{} arguments given", argc - 1);

    app->init();

    const uint32_t max_frame_count = 10;
    for(uint32_t frame_count = 0; frame_count < max_frame_count; frame_count++) {
      log::write<log::Level::kTrace>("Frame {}", frame_count);
      app->update();
    }

    app->shutdown();
  }
}