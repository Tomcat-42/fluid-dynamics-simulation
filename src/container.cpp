#include "container.hpp"
#include "ix.hpp"
#include <iostream>

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

sf::Color Container::hsv(int hue, float sat, float val, float d) {
  hue %= 360;
  while (hue < 0)
    hue += 360;

  if (sat < 0.f)
    sat = 0.f;
  if (sat > 1.f)
    sat = 1.f;

  if (val < 0.f)
    val = 0.f;
  if (val > 1.f)
    val = 1.f;

  int h = hue / 60;
  float f = float(hue) / 60 - h;
  float p = val * (1.f - sat);
  float q = val * (1.f - sat * f);
  float t = val * (1.f - sat * (1 - f));

  switch (h) {
  default:
  case 0:
  case 6:
    return sf::Color(val * 255, t * 255, p * 255, d);
  case 1:
    return sf::Color(q * 255, val * 255, p * 255, d);
  case 2:
    return sf::Color(p * 255, val * 255, t * 255, d);
  case 3:
    return sf::Color(p * 255, q * 255, val * 255, d);
  case 4:
    return sf::Color(t * 255, p * 255, val * 255, d);
  case 5:
    return sf::Color(val * 255, p * 255, q * 255, d);
  }
}

float Container::map_to_range(float val, float minIn, float maxIn, float minOut,
                              float maxOut) {
  float x = (val - minIn) / (maxIn - minIn);
  float result = minOut + (maxOut - minOut) * x;
  return (result < minOut) ? minOut : (result > maxOut) ? maxOut : result;
}

void Container::render(sf::RenderWindow &win, Color::ColorType color) {
  win.clear();
  for (int i = 0; i < this->size; i++) {
    for (int j = 0; j < this->size; j++) {
      sf::RectangleShape rect;
      rect.setSize(sf::Vector2f(SCALE, SCALE));
      rect.setPosition(j * SCALE, i * SCALE);

      switch (color) {
        case Color::ColorType::Default:
        rect.setFillColor(sf::Color(255, 255, 255,
                                    (this->density[IX(i, j, this->size)] > 255)
                                        ? 255
                                        : this->density[IX(i, j, this->size)]));
        break;
        case Color::ColorType::Hsb:
        rect.setFillColor(
            this->hsv((this->density[IX(i, j, this->size)]), 1, 1, 255));
        break;
        case Color::ColorType::Velocity: {
        int r = (int)this->map_to_range(this->x[IX(i, j, this->size)], -0.05f,
                                        0.05f, 0, 255);
        int g = (int)this->map_to_range(this->y[IX(i, j, this->size)], -0.05f,
                                        0.05f, 0, 255);
        rect.setFillColor(sf::Color(r, g, 255));
        break;
      }
      default:
        break;
      };

      win.draw(rect);
    }
  }
}

void Container::fade_density(int size) {
  for (int i = 0; i < size; i++) {
    float d = this->density[i];
    density[i] = (d - 0.05f < 0) ? 0 : d - 0.05f;
  }
}
