#include "container.hpp"
#include "color.hpp"
#include "ix.hpp"
#include <iostream>

#include "CImg/CImg.hpp"

Container::Container() : physics(Physics()) {}

Container::Container(float dt, float diff, float visc) {
  this->size = SIZE;
  this->dt = dt;
  this->diff = diff;
  this->visc = visc;

  this->init_array(this->px, SIZE * SIZE);
  this->init_array(this->py, SIZE * SIZE);
  this->init_array(this->x, SIZE * SIZE);
  this->init_array(this->y, SIZE * SIZE);
  this->init_array(this->previous_density, SIZE * SIZE);
  this->init_array(this->density, SIZE * SIZE);
}

Container::~Container() {}

void Container::init_array(float arr[], int size) {
  for (int i = 0; i < size; i++) {
    arr[i] = 0;
  }
}

void Container::add_density(float x, float y, float amount) {
  this->density[IX(x, y, this->size)] += amount;
}

void Container::add_velocity(float x, float y, float px, float py) {
  int index = IX(x, y, this->size);

  this->x[index] += px;
  this->y[index] += py;
}

void Container::step() {
  this->physics.Diffuse(1, this->px, this->x, this->visc, this->dt, 16,
                        this->size);
  this->physics.Diffuse(2, this->py, this->y, this->visc, this->dt, 16,
                        this->size);

  this->physics.Project(this->px, this->py, this->x, this->y, 16, this->size);

  this->physics.Advect(1, this->x, this->px, this->px, this->py, this->dt,
                       this->size);
  this->physics.Advect(2, this->y, this->py, this->px, this->py, this->dt,
                       this->size);

  this->physics.Project(this->x, this->y, this->px, this->py, 16, this->size);

  this->physics.Diffuse(0, this->previous_density, this->density, this->diff,
                        this->dt, 16, this->size);
  this->physics.Advect(0, this->density, this->previous_density, this->x,
                       this->y, this->dt, this->size);
}

float Container::map_to_range(float val, float minIn, float maxIn, float minOut,
                              float maxOut) {
  float x = (val - minIn) / (maxIn - minIn);
  float result = minOut + (maxOut - minOut) * x;
  return (result < minOut) ? minOut : (result > maxOut) ? maxOut : result;
}

void Container::render(cimg_library::CImg<unsigned char> &img,
                       Color::ColorType color_type) {

  for (int i = 0; i < this->size; i++) {
    for (int j = 0; j < this->size; j++) {
      // img.fill(0);

      Color::RgbaColor color_to_draw;

      switch (color_type) {
      case Color::ColorType::Default:
        color_to_draw =
            Color::RgbaColor(255, 255, 255,
                             (this->density[IX(i, j, this->size)] > 255)
                                 ? 255
                                 : this->density[IX(i, j, this->size)]);
        break;
      case Color::ColorType::Hsb:
        color_to_draw = Color::RgbaColor::from_hsv(
            this->density[IX(i, j, this->size)], 1, 1, 255);
        break;
      case Color::ColorType::Velocity: {
        int r = (int)this->map_to_range(this->x[IX(i, j, this->size)], -0.05f,
                                        0.05f, 0, 255);
        int g = (int)this->map_to_range(this->y[IX(i, j, this->size)], -0.05f,
                                        0.05f, 0, 255);
        color_to_draw = Color::RgbaColor(r, g, 255);
        break;
      }
      default:
        break;
      };

      const unsigned char colors[4] = {color_to_draw.r, color_to_draw.g,
                                       color_to_draw.b, color_to_draw.a};

      // std::cout << "colors: " << colors[0] << " " << colors[1] << " "
      //           << colors[2] << " " << colors[3] << std::endl;

      img.draw_rectangle(j * SCALE, i * SCALE, j * SCALE + SCALE,
                         i * SCALE + SCALE, colors);
    }
  }
}

void Container::fade_density(int size) {
  for (int i = 0; i < size; i++) {
    float d = this->density[i];
    density[i] = (d - 0.05f < 0) ? 0 : d - 0.05f;
  }
}
