// hello_world.cpp - A minimal app to demonstrate operation of rapier
#include <iostream>
#include <memory>

#include <rapier.hpp>

class HelloApp : public rp::App {
public:
  void init() {
    rp::log::info("Hello App Init Method");
  }

  void onEvent(const rp::Event& e) {
    //rp::log::trace("Event: {}", e);
  }

  void update() {
  }

  void shutdown() {
    rp::log::info("Hello App Shutdown Method");
  }
};

int main(int argc, char** argv) {
    rp::StartupProperties props;
    props.appName = "Hello World App";
    props.appVersion = { 0, 0, 1 };
    props.logClientPrefix = "HelloApp";
    auto windowProps = rp::Window::Properties{ "Hello World!!", 1280, 720 };
    props.windowProperties = windowProps;

    rp::run(std::make_unique<HelloApp>(), props);
    return 0;
}