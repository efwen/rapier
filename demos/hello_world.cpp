//hello_world.cpp - A minimal app to demonstrate operation of rapier
#include <iostream>
#include <rp/rapier.hpp>

int main() {
  std::cout << "Hello Rapier v" << rp::getVersion().toString() << "!\n";
  return 0;
}
