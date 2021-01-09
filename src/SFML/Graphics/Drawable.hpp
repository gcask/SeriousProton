#ifndef SERIOUS_PROTON_SFML_OVER_SDL_DRAWABLE_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_DRAWABLE_HPP
#include <SFML/Graphics/RenderStates.hpp>
namespace sf
{
	class RenderTarget;
	class Drawable
	{
	public:
		virtual ~Drawable();
		virtual void draw(RenderTarget&, RenderStates) const = 0;
	};
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_DRAWABLE_HPP