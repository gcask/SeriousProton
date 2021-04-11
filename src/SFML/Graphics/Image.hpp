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
		Image();
		Image(const Image&) = delete;
		Image& operator =(const Image&) = delete;
		Image(Image&&) = delete;
		Image& operator =(Image&&) = delete;

		const uint8_t* data() const { return pixels; }
		const uint8_t* getPixelsPtr() const { return data(); }
		size_t getByteSize() const { return byte_size; }
		uint32_t getFormat() const { return format; }
	private:
		void reset();
		uint8_t* pixels = nullptr;
		Vector2u size;
		size_t byte_size{};
		uint32_t format{UINT32_MAX};
	};
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_IMAGE_HPP