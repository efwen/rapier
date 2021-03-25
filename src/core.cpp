#include <iostream>

#include "rp/core.hpp"
#include "rp/log.hpp"
#include "rp/app.hpp"
#include "rp/version.hpp"

namespace rp {
  void run(std::unique_ptr<App> app, int argc, char** argv) {
    log::rp_info("--------------------------------------------");
    log::rp_info("Rapier v{} started!", getVersion().toString());
    log::rp_info("{} CLI arguments given", argc - 1);
    log::rp_info("--------------------------------------------\n");

    log::rp_info("--------------------------------------------");
    log::rp_info("Initializing Rapier!");

    app->init();

    log::rp_info("Initialization Complete!");
    log::rp_info("--------------------------------------------\n");

    const uint32_t max_frame_count = 10;
    for(uint32_t frame_count = 0; frame_count < max_frame_count; frame_count++) {
      log::rp_trace("Frame {}", frame_count);
      app->update();
    }

    log::rp_info("--------------------------------------------");
    log::rp_info("Shutting Down Rapier!");

    app->shutdown();

    log::rp_info("See you next time!");
    log::rp_info("--------------------------------------------\n");

  }
}