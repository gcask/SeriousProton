#ifndef SERIOUS_PROTON_SFML_OVER_SDL_TRANSFORMABLE_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_TRANSFORMABLE_HPP
#include <SFML/Graphics/Transform.hpp>
#include <glm/ext/matrix_float4x4.hpp>
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
	protected:
		// non sfml.
		glm::mat4 getTransform() const;
	public:
		void setTransform(const Transformable&);
	private:
		Vector2f scale = { 1.f, 1.f };
		Vector2f origin;
		Vector2f position;
		float rotation = 0.f;
	};
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_TRANSFORMABLE_HPP