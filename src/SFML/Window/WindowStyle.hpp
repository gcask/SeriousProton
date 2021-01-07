#ifndef SERIOUS_PROTON_SFML_OVER_SDL_WINDOWSTYLE_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_WINDOWSTYLE_HPP
#include <cstdint>
namespace sf
{
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

}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_WINDOWSTYLE_HPP