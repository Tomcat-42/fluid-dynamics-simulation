#include <iostream>
#include <cmath>
#include <cstdint>

#include "window.hpp"

int main(const __attribute__((unused)) int argc, const __attribute__((unused)) char** argv){
    Window window(500, 500);

    while (window.isOpen()){
        sf::Event event;
        while (window.pollEvent(event)){
            if (event.type == sf::Event::Closed){
                window.close();
            }
        }

        for (std::size_t i = 0; i < window.getX(); i++){
            for (std::size_t j = 0; j < window.getY(); j++){
                window.setPixel(i, j, sf::Color(255 * i / window.getX(), 255 * j / window.getY(), 255 * std::sin(i * j) * std::cos(i * j) * std::tan(i * j)));
            }
        }

        window.display();
    }
    return 0;
}
