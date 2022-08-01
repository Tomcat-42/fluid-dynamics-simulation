#include "simulation.hpp"
#include "CImg/CImg.hpp"
#include "const.hpp"
#include <iostream>

#include <sys/uio.h>

const int Simulation::num_particles = 100;

Simulation::Simulation()
    : container(Container(0.2f, 0, 0.0001f)),
      options(Options(Color::ColorType::Hsb)),
      img(cimg_library::CImg<unsigned char>(SIZE, SIZE, 1, 4, 0)) {}

Simulation::~Simulation() {}

void Simulation::setup() {}

void Simulation::run() {
  int center_x = SIZE / 2;
  int center_y = SIZE / 2;

  int radius = SIZE / 4;

  int x0 = center_x + (radius * cos(-0.1745329));
  int y0 = center_y + (radius * sin(-0.1745329));

  int x1 = center_x + (radius * cos(0));
  int y1 = center_y + (radius * sin(0));

  for (int i = 0; i < FRAMES; i++) {

    this->container.add_density(y0 / SCALE, x0 / SCALE, 200);

    float amountX = x1 - x0;
    float amountY = y1 - y0;

    this->container.add_velocity(y1 / SCALE, x1 / SCALE, amountY / 10,
                                 amountX / 10);

    this->container.step();
    this->container.render(this->img, this->options.get_color());
    this->container.fade_density(SIZE * SIZE);

    struct iovec iov[3];
    // ssize_t nwritten;
    iov[0].iov_base =
        reinterpret_cast<char *>(this->img.data() + (SIZE * SIZE));
    iov[0].iov_len = SIZE * SIZE;
    iov[1].iov_base =
        reinterpret_cast<char *>(this->img.data() + 2 * (SIZE * SIZE));
    iov[1].iov_len = SIZE * SIZE;
    iov[2].iov_base = reinterpret_cast<char *>(this->img.data());
    iov[2].iov_len = SIZE * SIZE;

    writev(STDOUT_FILENO, iov, 3);

    x0 = x1;
    y0 = y1;

    x1 = center_x + (radius * cos(i * (0.1745329)));
    y1 = center_y + (radius * sin(i * (0.1745329)));
  }
}
