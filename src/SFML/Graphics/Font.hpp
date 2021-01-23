#ifndef SERIOUS_PROTON_SFML_OVER_SDL_FONT_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_FONT_HPP
#include <SFML/Graphics/Glyph.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/String.hpp>

#include <memory>

namespace sf
{
	class Font final
	{
	public:
		Font();
		~Font();
		bool loadFromStream(InputStream& stream);
		float getLineSpacing(unsigned int characterSize) const;
		Glyph getGlyph(Uint32 codePoint, unsigned int characterSize, bool bold, float outlineThickness = 0) const;
	public:
		struct Impl;
		std::unique_ptr<Impl> impl;
	};
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_FONT_HPP