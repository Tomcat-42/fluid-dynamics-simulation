#include <iostream>
#include <cmath>
#include <cstdint>
#include <cstdlib>

#include "window.hpp"
#include "field.hpp"

#define SIZE 500

static inline __attribute__((always_inline))
std::uint16_t getAngle(double u, double v){
    double m = std::sqrt(std::pow(u, 2) + std::pow(v, 2));
    return (360 + (std::uint16_t)(std::atan2(v / m, u / m) * 180 / M_PI)) % 360;
}

static inline __attribute__((always_inline))
sf::Color hsv(std::int16_t hue, double sat, double val){
    if(sat<0.f) sat = 0.f;
    if(sat>1.f) sat = 1.f;

    if(val<0.f) val = 0.f;
    if(val>1.f) val = 1.f;

    int h = hue/60;
    float f = float(hue)/60-h;
    float p = val*(1.f-sat);
    float q = val*(1.f-sat*f);
    float t = val*(1.f-sat*(1-f));

    switch (h){
        default:
        case 0:
        case 6: return sf::Color(val*255, t*255, p*255);
        case 1: return sf::Color(q*255, val*255, p*255);
        case 2: return sf::Color(p*255, val*255, t*255);
        case 3: return sf::Color(p*255, q*255, val*255);
        case 4: return sf::Color(t*255, p*255, val*255);
        case 5: return sf::Color(val*255, p*255, q*255);
    }
}

int main(const __attribute__((unused)) int argc, const __attribute__((unused)) char** argv){
    Window window(SIZE, SIZE);

    Field<double> field(SIZE);

    auto [x0, y0] = window.mouseGetPosition();

    while (window.isOpen()){
        sf::Event event;
        while(window.pollEvent(event)){
            switch (event.type){
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::MouseButtonPressed:
                    if (event.mouseButton.button == sf::Mouse::Left){
                        auto [_x0, _y0] = window.mouseGetPosition();
                        x0 = _x0;
                        y0 = _y0;
                    }
                    break;
                default:
                    break;
            }
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)){

            auto [x, y] = window.mouseGetPosition();

            for (std::size_t i = 0; i < 25; i++){
                for (std::size_t j = 0; j < 25; j++){
                    auto [d, u, v] = field.operator[]<1>(std::min(x + i, (std::size_t)SIZE), std::min(y + j, (std::size_t)SIZE));

                    d += 0.5;

                    double p = ((double)x - (double)x0);
                    double q = ((double)y - (double)y0);
                    double m = std::sqrt(std::pow(p, 2) + std::pow(q, 2));

                    if (m != 0.0F){
                        u = p / m;
                        v = q / m;
                    }
                }
            }
        }

        field.step(0.00001, 0.00001, 0.1);

        for (std::size_t i = 0; i < SIZE; i++){
            for (std::size_t j = 0; j < SIZE; j++){

                auto [d, u, v] = field.operator[]<2>(i, j);

                double m = std::sqrt(std::pow(u, 2) + std::pow(v, 2));

                window.setPixel(i, j, hsv(getAngle(u / m, v / m), 1.0F, d));

                auto [dP, uP, vP] = field.operator[]<1>(i, j);

                dP = uP = vP = 0.0F;
            }
        }

        window.display();
    }
    return 0;
}
