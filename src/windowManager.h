#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H
#include <SFML/System/Vector2.hpp>
#include "sfml2sdl.h"
#ifndef WINDOW_TITLE
#define WINDOW_TITLE "SeriousProton Game"
#endif // WINDOW_TITLE

#include "P.h"
#include "Renderable.h"
#include "SDL_events.h"

class WindowManager : public virtual PObject
{
private:
    bool windowHasFocus;
    float min_aspect_ratio;
    bool allow_virtual_resize;

    sf::Vector2i virtualSize;
    sf::RenderWindow window;
    SDL_Window* sdl_window = nullptr;
    RenderChain* renderChain;
    bool fullscreen;
    int fsaa;
public:
    WindowManager(int virtualWidth, int virtualHeight, bool fullscreen, RenderChain* chain, int fsaa = 0);
    virtual ~WindowManager();

    sf::Vector2i getVirtualSize() const { return virtualSize; }
    void render();
    void close();
    bool hasFocus() { return windowHasFocus; }

    bool isFullscreen() { return fullscreen; }
    void setFullscreen(bool fullscreen);
    int getFSAA() { return fsaa; }
    void setFSAA(int fsaa);

    void setAllowVirtualResize(bool allow) { allow_virtual_resize = allow; setupView(); }
    void setFrameLimit(int limit) { window.setFramerateLimit(limit); }
    void setTitle(string title) { window.setTitle(title); }

    sf::Vector2f mapPixelToCoords(const sf::Vector2i& point) const;
    sf::Vector2i mapCoordsToPixel(const sf::Vector2f& point) const;
    bool pollEvent(SDL_Event&);
    friend class InputHandler;
    friend class Engine;
    friend class Clipboard;
    
private:
    void create();
    void setupView();
};

#endif//WINDOW_MANAGER_H
