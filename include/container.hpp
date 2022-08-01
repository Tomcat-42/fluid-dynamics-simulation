#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

#include "const.hpp"
#include "options.hpp"
#include "physics.hpp"

#include "CImg/CImg.hpp"

class Container {
private:
  Physics physics;

  int size;

  float dt;
  float diff;
  float visc;

  float px[SIZE * SIZE];
  float py[SIZE * SIZE];

  float x[SIZE * SIZE];
  float y[SIZE * SIZE];

  float previous_density[SIZE * SIZE];
  float density[SIZE * SIZE];

  void init_array(float arr[], int size);
  float map_to_range(float value, float min_in, float max_in, float min_out,
                     float max_out);

public:
  Container();
  Container(float dt, float diff, float visc);
  ~Container();

  void add_density(float x, float y, float amount);
  void add_velocity(float x, float y, float px, float py);
  void step();
  void fade_density(int size);

  void render(cimg_library::CImg<unsigned char> &img,
              Color::ColorType color_type);
};
