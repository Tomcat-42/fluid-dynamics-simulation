#include "options.hpp"

Options::Options(Color::ColorType color) { this->color = color; }

Options::~Options() {}

Color::ColorType Options::get_color() { return this->color; }

void Options::set_color(Color::ColorType c) { this->color = c; }
