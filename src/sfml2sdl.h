#ifndef SERIOUS_PROTON_SFML2SDL_H
#define SERIOUS_PROTON_SFML2SDL_H
#include <cstdint>
#include "stringImproved.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/ContextSettings.hpp>

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

namespace Style
{
    enum
    {
        None = 0,
        Titlebar = 1 << 0,
        Resize = 1 << 1,
        Close = 1 << 2,
        Fullscreen = 1 << 3,

        Default = Titlebar | Resize | Close
    };
}

    struct VideoMode
    {
        VideoMode(uint32_t modeWidth, uint32_t modeHeight, uint32_t modeBitsPerPixel = 32);
        static VideoMode getDesktopMode();
        uint32_t width = 0;
        uint32_t height = 0;
    };

	class RenderWindow final : public sf::RenderTarget
    {
    public:
        void setFramerateLimit(int32_t);
        void setTitle(const string&);
        bool isOpen() const;
        const sf::View &getView() const;
        sf::Vector2u getSize() const override final;
        void *getSystemHandle() const;
        void display();
        void close();

        void create(VideoMode mode, const string& title, Uint32 style = Style::Default, const ContextSettings& settings = ContextSettings());
        const ContextSettings& getSettings() const;
        void setVerticalSyncEnabled(bool);
        void setMouseCursorVisible(bool);
    };

    namespace Mouse
    {
        sf::Vector2i getPosition(const RenderWindow &);
        void setPosition(const sf::Vector2i&, RenderWindow&);
    }
}
#endif // SERIOUS_PROTON_SFML2SDL_H