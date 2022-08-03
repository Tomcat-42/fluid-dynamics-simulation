#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdint>

#include "simulation.hpp"

// Wrapper class for the SFML RenderWindow
class Window {
    sf::RenderWindow *window;
    sf::Image buffer;
    sf::Texture texture;
    sf::Sprite sprite;

    std::size_t windowX;
    std::size_t windowY;

public:

    std::int16_t getAngle(const double& u, const double& v);

    sf::Color hsv(const std::int16_t& hue, double sat, double val);

    Window(const std::size_t& windowX, const std::size_t& windowY);

    std::size_t getX() const;
    std::size_t getY() const;

    bool isOpen() const;
    bool pollEvent(sf::Event& event);

    void close();
    void display();
    void setPixel(const std::size_t& x, const std::size_t& y, const sf::Color& color);

    std::tuple<std::size_t, std::size_t> mouseGetPosition();

    ~Window();
};
