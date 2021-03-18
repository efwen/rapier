// hello_world.cpp - A minimal app to demonstrate operation of rapier
#include <iostream>
#include <memory>

#include <rp/rapier.hpp>

class HelloApp : public rp::App {
public:
  void init() {
    std::cout << "Hello App init method\n";
  }

  void update() {
    std::cout << "Hello update method\n";
  }

  void shutdown() {
    std::cout << "Hello App shutdown method\n";
  }
};

int main() {
  std::cout << "Hello Rapier v" << rp::getVersion().toString() << "!\n";

  auto app = std::make_unique<HelloApp>();
  app->init();

  const uint32_t max_frame_count = 40;
  for(uint32_t frame_count = 0; frame_count < max_frame_count; frame_count++) {
    std::cout << "frame " << frame_count << '\n';
    app->update();
  }

  app->shutdown();

  return 0;
}
