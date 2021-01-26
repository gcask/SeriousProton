#ifndef SERIOUS_PROTON_SFML_OVER_SDL_RENDERWINDOW_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_RENDERWINDOW_HPP
#include <cstdint>
#include <memory>
#include <string>
#include <SFML/Window/ContextSettings.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowStyle.hpp>
namespace sf
{
    class RenderWindow final : public sf::RenderTarget
    {
    public:
        struct Impl;
        RenderWindow();
        ~RenderWindow();
        void setFramerateLimit(int32_t);
        void setTitle(const std::string&);
        bool isOpen() const;
        sf::Vector2u getSize() const override final;
        void display();
        void close();

        void create(VideoMode mode, const std::string& title, Uint32 style = Style::Default, const ContextSettings& settings = ContextSettings());
        const ContextSettings& getSettings() const;
        void setVerticalSyncEnabled(bool);
        void setMouseCursorVisible(bool);
        Impl *getImpl() const;
    private:
        
        std::unique_ptr<Impl> impl;
    };

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
        sf::Vector2i getPosition(const RenderWindow&);
        void setPosition(const sf::Vector2i&, RenderWindow&);
    }
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_RENDERWINDOW_HPP