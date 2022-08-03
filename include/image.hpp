#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdint>
#include <tuple>

namespace Image {
    // NOTE: ffmpeg expects RGBA
    using Color = std::tuple<std::uint8_t, std::uint8_t, std::uint8_t, std::uint8_t>;

    std::uint8_t* genImage(std::size_t size, const double* d, const double* v, const double* u);

    Color hsv(const std::int16_t& hue, double sat, double val);

    std::int16_t getAngle(const double& u, const double& v);
}
