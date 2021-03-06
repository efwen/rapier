#include "pch.hpp"

#include "core/core.hpp"
#include "core/app.hpp"
#include "core/window.hpp"
#include "util/version.hpp"


namespace rp {
  void run(std::unique_ptr<App> app, StartupProperties startupProperties) {
    try {
      log::rp_info(log::horiz_rule);
      log::rp_info("Rapier v{} started!", getVersion().toString());
      log::setClientPrefix(startupProperties.logClientPrefix);

      log::rp_info(log::horiz_rule);
      log::rp_info("Initializing Rapier!");
      log::rp_info(log::horiz_rule);

      auto window = createWindow(startupProperties.windowProperties);
      window->setCallback([&](const Event& e) { app->onEvent(e); });

      app->init();

      log::rp_info(log::horiz_rule);
      log::rp_info("Initialization Complete!");
      log::rp_info("{}{}", log::horiz_rule, log::new_line);




      bool running = true;
      while(running) {
        app->update();
        running = window->processMessages();
      }




      log::rp_info(log::horiz_rule);
      log::rp_info("Shutting Down Rapier!");
      log::rp_info(log::horiz_rule);

      app->shutdown();

      log::rp_info(log::horiz_rule);
      log::rp_info("See you next time!");
      log::rp_info(log::horiz_rule);
    } catch(std::exception& e) {
      log::rp_error(log::horiz_rule);
      log::rp_error("UNCAUGHT EXCEPTION!: {}", e.what());
      log::rp_error(log::horiz_rule);
      exit(-1);
    }
  }
}