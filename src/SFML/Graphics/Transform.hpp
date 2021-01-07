#ifndef SERIOUS_PROTON_SFML_OVER_SDL_TRANSFORM_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_TRANSFORM_HPP
#include <cstdint>
#include <SFML/System/Vector2.hpp>
#include <glm/mat4x4.hpp>
namespace sf
{
	class Transform
	{
	public:
		Transform();
		Transform(float a00, float a01, float a02,
			float a10, float a11, float a12,
			float a20, float a21, float a22);
		Vector2f transformPoint(const Vector2f& point) const;
	private:
		glm::mat4 matrix;
	};
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_TRANSFORM_HPP