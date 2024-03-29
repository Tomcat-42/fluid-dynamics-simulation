#include "image.hpp"
#include <algorithm>

void Image::genImage(std::uint32_t* image, std::size_t size, const double* d, const double* v, const double* u){
    for (std::size_t i = 0; i < (size + 2) * (size + 2); i++)
        image[i] = Image::hsv(Image::getAngle(u[i], v[i]), 1.0, d[i]);
}

inline __attribute__((always_inline))
std::uint32_t Image::hsv(const std::int16_t& hue, double sat, double val){
    sat = std::clamp(sat, (double)0.0, (double)1.0);
    val = std::clamp(val, (double)0.0, (double)1.0);

    std::int16_t h = hue / 60;
    double f = (double)hue / 60 - h;
    double p = val * (1.0F - sat);
    double q = val * (1.0F - sat * f);
    double t = val * (1.0F - sat * (1.0F - f));

    // NOTE: since we save the color as a uint32_t (RGBA), and the architecture is little endian we need to do it the other way around
    switch (h){
        default:
        case 0:
        case 6: return ((std::uint32_t)(val * 0xFF)) | ((std::uint32_t)(  t * 0xFF) << 8) | ((std::uint32_t)(  p * 0xFF) << 16) | ((std::uint32_t)0xFF << 24);
        case 1: return ((std::uint32_t)(  q * 0xFF)) | ((std::uint32_t)(val * 0xFF) << 8) | ((std::uint32_t)(  p * 0xFF) << 16) | ((std::uint32_t)0xFF << 24);
        case 2: return ((std::uint32_t)(  p * 0xFF)) | ((std::uint32_t)(val * 0xFF) << 8) | ((std::uint32_t)(  t * 0xFF) << 16) | ((std::uint32_t)0xFF << 24);
        case 3: return ((std::uint32_t)(  p * 0xFF)) | ((std::uint32_t)(  q * 0xFF) << 8) | ((std::uint32_t)(val * 0xFF) << 16) | ((std::uint32_t)0xFF << 24);
        case 4: return ((std::uint32_t)(  t * 0xFF)) | ((std::uint32_t)(  p * 0xFF) << 8) | ((std::uint32_t)(val * 0xFF) << 16) | ((std::uint32_t)0xFF << 24);
        case 5: return ((std::uint32_t)(val * 0xFF)) | ((std::uint32_t)(  p * 0xFF) << 8) | ((std::uint32_t)(  q * 0xFF) << 16) | ((std::uint32_t)0xFF << 24);
    }
}

inline __attribute__((always_inline))
std::int16_t Image::getAngle(const double& u, const double& v){
    double m = std::sqrt(std::pow(u, 2) + std::pow(v, 2));
    return (360 + (std::int16_t)(std::atan2(v / m, u / m) * 180 / M_PI)) % 360;
}
