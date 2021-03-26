#include "pch.hpp"

#include "core.hpp"
#include "app.hpp"
#include "version.hpp"

namespace rp {
  void run(std::unique_ptr<App> app, int argc, char** argv) {
    log::rp_info(log::horiz_rule);
    log::rp_info("Rapier v{} started!", getVersion().toString());
    log::rp_info("{} CLI arguments given", argc - 1);
    log::rp_info("{}{}", log::horiz_rule, log::new_line);

    log::rp_info(log::horiz_rule);
    log::rp_info("Initializing Rapier!");

    app->init();

    log::rp_info("Initialization Complete!");
    log::rp_info("{}{}", log::horiz_rule, log::new_line);

    const uint32_t max_frame_count = 10;
    for(uint32_t frame_count = 0; frame_count < max_frame_count; frame_count++) {
      log::rp_trace("Frame {}", frame_count);
      app->update();
    }

    log::rp_info(log::horiz_rule);
    log::rp_info("Shutting Down Rapier!");

    app->shutdown();

    log::rp_info("See you next time!");
    log::rp_info(log::horiz_rule);

  }
}