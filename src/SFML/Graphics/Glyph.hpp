#ifndef SERIOUS_PROTON_SFML_OVER_SDL_GLYPH_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_GLYPH_HPP
#include <SFML/Graphics/Rect.hpp>
namespace sf
{
	class Glyph
	{
	public:
		float     advance = 0.f;     ///< Offset to move horizontally to the next character
		FloatRect bounds;      ///< Bounding rectangle of the glyph, in coordinates relative to the baseline
		IntRect   textureRect; ///< Texture coordinates of the glyph inside the font's texture
	};
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_GLYPH_HPP