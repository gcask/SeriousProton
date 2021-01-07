#ifndef SERIOUS_PROTON_SFML_OVER_SDL_VERTEX_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_VERTEX_HPP
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

namespace sf
{
	class Vertex
	{
	public:
		Vertex();

		Vector2f  position;
		Color     color;
		Vector2f  texCoords;
	};
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_VERTEX_HPP