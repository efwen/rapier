#include <iostream>

#include "rp/core.hpp"
#include "rp/app.hpp"
#include "rp/version.hpp"

namespace rp {
  void run(std::unique_ptr<App> app, int argc, char** argv) {
    std::cout << "Rapier v" << getVersion().toString() << " started!\n";
    std::cout << argc - 1 << " arguments given\n";

    app->init();

    const uint32_t max_frame_count = 10;
    for(uint32_t frame_count = 0; frame_count < max_frame_count; frame_count++) {
      std::cout << "frame " << frame_count << '\n';
      app->update();
    }

    app->shutdown();
  }
}