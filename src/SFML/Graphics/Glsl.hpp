#ifndef SERIOUS_PROTON_SFML_OVER_SDL_GLSL_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_GLSL_HPP
#include <SFML/Graphics/Transform.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
namespace sf
{
	namespace Glsl
	{
		struct Vec4
		{
			Vec4(const Color&);
		};
	}
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_GLSL_HPP