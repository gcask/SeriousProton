#ifndef SERIOUS_PROTON_SFML2SDL_H
#define SERIOUS_PROTON_SFML2SDL_H
#include <cstdint>
#include <memory>
#include "stringImproved.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/ContextSettings.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

namespace sf
{
    namespace Joystick
    {
        enum Axis
        {
            X = 0,    ///< The X axis
            Y,    ///< The Y axis
            Z,    ///< The Z axis
            R,    ///< The R axis
            U,    ///< The U axis
            V,    ///< The V axis
            PovX, ///< The X axis of the point-of-view hat
            PovY,  ///< The Y axis of the point-of-view hat

            AxisCount ///< Axis count (must be last!)
        };
        static constexpr uint32_t ButtonCount = 32;
    }

    namespace Mouse
    {
        sf::Vector2i getPosition(const RenderWindow &);
        void setPosition(const sf::Vector2i&, RenderWindow&);
    }
}
#endif // SERIOUS_PROTON_SFML2SDL_H