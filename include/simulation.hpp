#pragma once
#include <vector>

#include "container.hpp"

#include "CImg/CImg.hpp"

class Simulation {
private:
  Container container;
  Options options;
  cimg_library::CImg<unsigned char> img;
  void setup();

public:
  static const int num_particles;
  Simulation();
  ~Simulation();
  void run();
};
