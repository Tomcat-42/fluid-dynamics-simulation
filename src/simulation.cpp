#include "simulation.hpp"
#include <iostream>

const int Simulation::num_particles = 100;

Simulation::Simulation()
    : container(Container(0.2f, 0, 0.0001f)), options(Options()) {
  this->win.create(sf::VideoMode(SIZE * SCALE, SIZE * SCALE),
                   "Euler fluid simulation - Github: "
                   "Gimp",
                   sf::Style::Titlebar | sf::Style::Close);
}

Simulation::~Simulation() {}

void Simulation::setup() {}

void Simulation::run() {
  this->setup();
  sf::Vector2i previous_mouse = sf::Mouse::getPosition(this->win);
  sf::Vector2i current_mouse = sf::Mouse::getPosition(this->win);

  while (this->win.isOpen()) {
    sf::Event e;
    while (this->win.pollEvent(e)) {
      switch (e.type) {
      case sf::Event::Closed:
        this->win.close();
        break;
      case sf::Event::KeyReleased:
        if (e.key.code == sf::Keyboard::Key::C) {
          Color::ColorType c =
              (this->options.get_color() == Color::ColorType::Default)
                  ? Color::ColorType::Hsb
              : (this->options.get_color() == Color::ColorType::Hsb)
                  ? Color::ColorType::Velocity
                  : Color::ColorType::Default;

          this->options.set_color(c);
        }
        break;
      default:
        break;
      }
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
      this->container.add_density(current_mouse.y / SCALE,
                                  current_mouse.x / SCALE, 200);

    current_mouse = sf::Mouse::getPosition(this->win);

    float amountX = current_mouse.x - previous_mouse.x;
    float amountY = current_mouse.y - previous_mouse.y;

    this->container.add_velocity(current_mouse.y / SCALE,
                                 current_mouse.x / SCALE, amountY / 10,
                                 amountX / 10);

    previous_mouse = current_mouse;

    this->container.step();
    this->container.render(this->win, this->options.get_color());
    this->container.fade_density(SIZE * SIZE);

    this->win.display();
  }
}
