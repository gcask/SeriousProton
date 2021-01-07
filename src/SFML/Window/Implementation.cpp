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
	VideoMode::VideoMode(uint32_t modeWidth, uint32_t modeHeight, uint32_t modeBitsPerPixel)
    {
        throw not_implemented();
    }

	VideoMode VideoMode::getDesktopMode()
    {
        throw not_implemented();
    }
}