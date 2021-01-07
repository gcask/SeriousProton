#ifndef SERIOUS_PROTON_SFML_OVER_SDL_RENDERWINDOW_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_RENDERWINDOW_HPP
#include <cstdint>
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
        RenderWindow();
        ~RenderWindow();
        void setFramerateLimit(int32_t);
        void setTitle(const std::string&);
        bool isOpen() const;
        const sf::View& getView() const;
        sf::Vector2u getSize() const override final;
        void* getSystemHandle() const;
        void display();
        void close();

        void create(VideoMode mode, const std::string& title, Uint32 style = Style::Default, const ContextSettings& settings = ContextSettings());
        const ContextSettings& getSettings() const;
        void setVerticalSyncEnabled(bool);
        void setMouseCursorVisible(bool);
    private:
        struct Impl;
        std::unique_ptr<Impl> impl;
    };
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_RENDERWINDOW_HPP