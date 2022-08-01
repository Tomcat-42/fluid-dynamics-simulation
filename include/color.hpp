#pragma once
namespace Color {
enum class ColorType { Default, Hsb, Velocity };
class Color {
  int r, g, b, a;

public:
  Color(int r, int g, int b, int a = 255) : r(r), g(g), b(b), a(a) {}
};
} // namespace Color
