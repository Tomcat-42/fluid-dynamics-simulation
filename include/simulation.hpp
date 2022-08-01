#pragma once
#include <vector>

#include "container.hpp"

class Simulation {
private:
  sf::RenderWindow win;
  Container container;
  Options options;
  void setup();

public:
  static const int num_particles;
  Simulation();
  ~Simulation();
  void run();
};
