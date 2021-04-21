// hello_world.cpp - A minimal app to demonstrate operation of rapier
#include <iostream>
#include <memory>

#include <rapier.hpp>

class HelloApp : public rp::App {
public:
  HelloApp() {
    rp::log::setClientPrefix("HelloApp");
  }

  void init() {
    rp::log::info("Hello App Init Method");
  }

  void onEvent(const rp::Event& e) {
    rp::log::info("Event: {}", e);
  }

  void update() {
  }

  void shutdown() {
    rp::log::info("Hello App Shutdown Method");
  }
};

int main(int argc, char** argv) {
    rp::run(std::make_unique<HelloApp>(), argc, argv);
    return 0;
}