#ifndef SERIOUS_PROTON_SFML_OVER_SDL_GLSL_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_GLSL_HPP
#include <SFML/Graphics/Transform.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

#include <initializer_list>
namespace sf
{
	namespace Glsl
	{
		struct Vec4
		{
			Vec4(float x, float y, float z, float w);
			Vec4(const Color&);
			glm::vec4 vector;
		};
	}
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_GLSL_HPP