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

int main(int argc, char** argv) {
  rp::run(std::make_unique<HelloApp>(), argc, argv);
  return 0;
}