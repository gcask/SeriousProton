#ifndef SERIOUS_PROTON_SFML_OVER_SDL_FONT_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_FONT_HPP
#include <SFML/Graphics/Glyph.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/String.hpp>

#include "SDL_ttf.h"
namespace sf
{
	class Font
	{
	public:
		bool loadFromStream(InputStream& stream);
		float getLineSpacing(unsigned int characterSize) const;
		const Glyph& getGlyph(Uint32 codePoint, unsigned int characterSize, bool bold, float outlineThickness = 0) const;
		TTF_Font* sdlObject = nullptr;
	};
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_FONT_HPP