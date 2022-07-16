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


Window::~Window(){}
