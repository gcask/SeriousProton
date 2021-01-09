#ifndef SERIOUS_PROTON_SFML_OVER_SDL_TEXTURE_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_TEXTURE_HPP
#include <cstdint>
#include <SFML/Graphics/Image.hpp>

#include "SDL_surface.h"
namespace sf
{
	class Texture
	{
	public:
		enum CoordinateType
		{
			Normalized, ///< Texture coordinates in range [0 .. 1]
			Pixels      ///< Texture coordinates in range [0 .. size]
		};

		static void bind(const Texture* texture, CoordinateType coordinateType = Normalized);
		virtual ~Texture();
		virtual Vector2u getSize() const;
		bool loadFromImage(const Image& image, const IntRect& area = IntRect());
		void setRepeated(bool repeated);
		void setSmooth(bool smooth);
		
		SDL_Surface *sdlObject = nullptr;
	};
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_TEXTURE_HPP