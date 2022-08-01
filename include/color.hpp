#pragma once

namespace Color {
enum class ColorType { Default, Hsb, Velocity };
struct RgbaColor {
  unsigned char r, g, b, a;

  RgbaColor(unsigned char r = 255, unsigned char g = 255, unsigned char b = 255,
            unsigned char a = 255)
      : r(r), g(g), b(b), a(a) {}

  static RgbaColor from_hsv(int hue, float sat, float val, float d);
};
} // namespace Color
