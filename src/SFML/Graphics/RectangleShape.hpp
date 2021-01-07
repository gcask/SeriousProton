#ifndef SERIOUS_PROTON_SFML_OVER_SDL_RECTANGLESHAPE_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_RECTANGLESHAPE_HPP
#include <SFML/Graphics/Shape.hpp>
namespace sf
{
	class RectangleShape : public Shape
	{
	public:
		explicit RectangleShape(const Vector2f& size = Vector2f(0, 0));
		void setSize(const Vector2f& size);
		const Vector2f& getSize() const;
	};
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_RECTANGLESHAPE_HPP