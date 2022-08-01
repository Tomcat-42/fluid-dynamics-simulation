#pragma once
#include "color.hpp"

class Options {
private:
  Color::ColorType color;

public:
  Options(Color::ColorType color = Color::ColorType::Default);
  ~Options();

  Color::ColorType get_color();
  void set_color(Color::ColorType c);
};
