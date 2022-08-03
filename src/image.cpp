#include "image.hpp"

std::uint8_t* Image::genImage(std::size_t size, const double* d, const double* v, const double* u){
    // NOTE: the array does include the borders
    std::uint8_t* image = new std::uint8_t[(size + 2) * (size + 2) * 4];

    // TODO: fix this
// #pragma omp parallel
//   {
// #pragma omp for simd
        for (std::size_t i = 0, j = 0; i < (size + 2) * (size + 2); i++, j += 4){
            auto [r, g, b, a] = Image::hsv(Image::getAngle(u[i], v[i]), 1.0, d[i]);
            image[j]     = r;
            image[j + 1] = g;
            image[j + 2] = b;
            image[j + 3] = a;
        }
//    }
    // TODO: we should not return memory that we allocated
    return image;
}

inline __attribute__((always_inline))
Image::Color Image::hsv(const std::int16_t& hue, double sat, double val){
    sat = std::clamp(sat, (double)0.0, (double)1.0);
    val = std::clamp(val, (double)0.0, (double)1.0);

    std::int16_t h = hue / 60;
    double f = (double)hue / 60 - h;
    double p = val * (1.0F - sat);
    double q = val * (1.0F - sat * f);
    double t = val * (1.0F - sat * (1.0F - f));

    switch (h){
        default:
        case 0:
        case 6: return {val * 255,   t * 255,    p * 255, 255};
        case 1: return {  q * 255, val * 255,    p * 255, 255};
        case 2: return {  p * 255, val * 255,    t * 255, 255};
        case 3: return {  p * 255,   q * 255,  val * 255, 255};
        case 4: return {  t * 255,   p * 255,  val * 255, 255};
        case 5: return {val * 255,   p * 255,    q * 255, 255};
    }
}

inline __attribute__((always_inline))
std::int16_t Image::getAngle(const double& u, const double& v){
    double m = std::sqrt(std::pow(u, 2) + std::pow(v, 2));
    return (360 + (std::int16_t)(std::atan2(v / m, u / m) * 180 / M_PI)) % 360;
}
