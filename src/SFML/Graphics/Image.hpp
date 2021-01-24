#ifndef SERIOUS_PROTON_SFML_OVER_SDL_IMAGE_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_IMAGE_HPP
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

namespace sf
{
	class InputStream;
	class Image final
	{
	public:
		void create(unsigned int width, unsigned int height, const Color& color = Color(0, 0, 0));
		Vector2u getSize() const;
		bool loadFromStream(InputStream& stream);
		Color getPixel(uint32_t x, uint32_t y) const;

		~Image();
		const uint8_t* data() const { return pixels; }
	private:
		void reset();
		uint8_t* pixels = nullptr;
		Vector2u size;
	};
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_IMAGE_HPP