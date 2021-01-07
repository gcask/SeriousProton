#ifndef SERIOUS_PROTON_SFML_OVER_SDL_TRANSFORMABLE_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_TRANSFORMABLE_HPP
#include <SFML/Graphics/Transform.hpp>
namespace sf
{
	class Transformable
	{
	public:
		virtual ~Transformable();
		void setScale(float factorX, float factorY);
		void setOrigin(float x, float y);
		void setOrigin(const Vector2f& position);
		void setPosition(float x, float y);
		void setPosition(const Vector2f& position);
		void setRotation(float angle);
	};
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_TRANSFORMABLE_HPP