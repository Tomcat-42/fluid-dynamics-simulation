#include "window.hpp"

Window::Window(const std::size_t& windowX, const std::size_t& windowY): windowX(windowX), windowY(windowY) {
    this->window = new sf::RenderWindow(sf::VideoMode(windowX, windowY), "Gimp"); // NOTE: dwm kinda stinky
        this->buffer.create(windowX, windowY);
}

std::size_t Window::getX() const {
    return this->windowX;
}

std::size_t Window::getY() const {
    return this->windowY;
}

bool Window::isOpen(void) const {
    return this->window->isOpen();
}

bool Window::pollEvent(sf::Event& event){
    return this->window->pollEvent(event);
}

void Window::close(){
    this->window->close();
}

void Window::display(void){
    this->texture.loadFromImage(buffer);
    this->sprite.setTexture(texture);
    this->window->draw(sprite);
    this->window->display();
}

void Window::setPixel(const std::size_t& x, const std::size_t& y, const sf::Color& color){
    buffer.setPixel(x, y, color);
}

std::tuple<std::size_t, std::size_t> Window::mouseGetPosition(){
    sf::Vector2i mousePosition = sf::Mouse::getPosition(*this->window);
    return { mousePosition.x, mousePosition.y };
}

std::int16_t Window::getAngle(const double& u, const double& v){
    double m = std::sqrt(std::pow(u, 2) + std::pow(v, 2));
    return (360 + (std::int16_t)(std::atan2(v / m, u / m) * 180 / M_PI)) % 360;
}

sf::Color Window::hsv(const std::int16_t& hue, double sat, double val){
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
        case 6: return sf::Color(val * 255,   t * 255,    p * 255);
        case 1: return sf::Color(  q * 255, val * 255,    p * 255);
        case 2: return sf::Color(  p * 255, val * 255,    t * 255);
        case 3: return sf::Color(  p * 255,   q * 255,  val * 255);
        case 4: return sf::Color(  t * 255,   p * 255,  val * 255);
        case 5: return sf::Color(val * 255,   p * 255,    q * 255);
    }
}

Window::~Window(){}
