#include "sfml2sdl.h"
namespace sf
{

    VideoMode::VideoMode(uint32_t modeWidth, uint32_t modeHeight, uint32_t modeBitsPerPixel /*= 32*/)
        :width{ modeWidth }
        , height{ modeHeight }
    {

    }

    VideoMode VideoMode::getDesktopMode()
    {
        return VideoMode(0, 0);
    }

    void RenderWindow::setFramerateLimit(int32_t)
    {

    }
    void RenderWindow::setTitle(const string&)
    {

    }
    bool RenderWindow::isOpen() const
    {
        return false;
    }

    const sf::View& RenderWindow::getView() const
    {
        return sf::View();
    }
    sf::Vector2u RenderWindow::getSize() const
    {
        return sf::Vector2u(0u, 0u);
    }
    void* RenderWindow::getSystemHandle() const
    {
        return nullptr;
    }
    void RenderWindow::display()
    {

    }
    void RenderWindow::close()
    {

    }

    void RenderWindow::create(VideoMode mode, const string& title, Uint32 style /*= Style::Default*/, const ContextSettings& settings /*= ContextSettings()*/)
    {

    }

    const ContextSettings& RenderWindow::getSettings() const
    {
        return ContextSettings();
    }
    void RenderWindow::setVerticalSyncEnabled(bool)
    {

    }
    void RenderWindow::setMouseCursorVisible(bool)
    {

    }

    sf::Vector2i Mouse::getPosition(const RenderWindow&)
    {
        return sf::Vector2i(0, 0);
    }

    void Mouse::setPosition(const sf::Vector2i&, RenderWindow&)
    {

    }
}