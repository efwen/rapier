// hello_world.cpp - A minimal app to demonstrate operation of rapier
#include <iostream>
#include <memory>

#include <rp/rapier.hpp>

class HelloApp : public rp::App {
public:
  void init() {
    rp::log::write<rp::log::Level::kTrace>("Hello App Init Method");
  }

  void update() {
    rp::log::write<rp::log::Level::kTrace>("Hello App Update Method");
  }

  void shutdown() {
    rp::log::write<rp::log::Level::kTrace>("Hello App Shutdown Method");
  }
};

int main(int argc, char** argv) {
  rp::run(std::make_unique<HelloApp>(), argc, argv);
  return 0;
}