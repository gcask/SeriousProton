#include "ContextSettings.hpp"
#include "VideoMode.hpp"
#include "WindowStyle.hpp"

#include "SDL.h"
#include <stdexcept>
namespace sf
{
    class not_implemented : public std::runtime_error
    {
    public:
        not_implemented()
            :std::runtime_error("not implemented")
        {}
    };
	// VideoMode
	VideoMode::VideoMode(uint32_t modeWidth, uint32_t modeHeight, uint32_t /*modeBitsPerPixel*/)
        :width(modeWidth), height(modeHeight)
    {
        sdlObject.h = height;
        sdlObject.w = width;
    }

	VideoMode VideoMode::getDesktopMode()
    {
        VideoMode desktop(0, 0);
        if (!SDL_GetDesktopDisplayMode(0, &desktop.sdlObject))
        {
            desktop.width = desktop.sdlObject.w;
            desktop.height = desktop.sdlObject.h;
        }

        return desktop;
    }
}