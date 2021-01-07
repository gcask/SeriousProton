#include "sfml2sdl.h"
namespace sf
{
    sf::Vector2i Mouse::getPosition(const RenderWindow&)
    {
        return sf::Vector2i(0, 0);
    }

    void Mouse::setPosition(const sf::Vector2i&, RenderWindow&)
    {

    }
}