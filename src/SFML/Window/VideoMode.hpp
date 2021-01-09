#ifndef SERIOUS_PROTON_SFML_OVER_SDL_VIDEOMODE_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_VIDEOMODE_HPP
#include <cstdint>

#include "SDL_video.h"
namespace sf
{
    struct VideoMode
    {
        VideoMode(uint32_t modeWidth, uint32_t modeHeight, uint32_t modeBitsPerPixel = 32);
        static VideoMode getDesktopMode();
        uint32_t width = 0;
        uint32_t height = 0;
        SDL_DisplayMode sdlObject;
    };
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_VIDEOMODE_HPP