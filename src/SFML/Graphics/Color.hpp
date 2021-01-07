#ifndef SERIOUS_PROTON_SFML_OVER_SDL_COLOR_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_COLOR_HPP
#include <cstdint>
#include "SDL_pixels.h"
namespace sf
{
	struct Color : public SDL_Color
	{
		Color();
		Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255);
		explicit Color(Uint32 color);

		static const Color White;
		static const Color Black;
		static const Color Red;
		static const Color Green;
		static const Color Blue;
		static const Color Yellow;
		static const Color Magenta;
		static const Color Cyan;
		static const Color Transparent;
	};

	bool operator ==(const Color& left, const Color& right);
	bool operator !=(const Color& left, const Color& right);
	Color operator -(const Color& left, const Color& right);
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_COLOR_HPP