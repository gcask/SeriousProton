#ifndef SERIOUS_PROTON_SFML_OVER_SDL_IMAGE_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_IMAGE_HPP
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

#include "SDL_surface.h"
namespace sf
{
	class InputStream;
	class Image
	{
	public:
		void create(unsigned int width, unsigned int height, const Color& color = Color(0, 0, 0));
		bool loadFromImage(const Image& image, const IntRect& area = IntRect());
		Vector2u getSize() const;
		bool loadFromStream(InputStream& stream);
		Color getPixel(unsigned int x, unsigned int y) const;
		SDL_Surface* sdlObject = nullptr;
	};
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_IMAGE_HPP