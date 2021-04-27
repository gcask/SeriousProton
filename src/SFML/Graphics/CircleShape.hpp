#ifndef SERIOUS_PROTON_SFML_OVER_SDL_CIRCLESHAPE_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_CIRCLESHAPE_HPP
#include <cstdint>
#include <cstddef>
#include <SFML/Graphics/Shape.hpp>
namespace sf
{
	class CircleShape : public Shape
	{
	public:
		explicit CircleShape(float radius = 0, std::size_t pointCount = 30);
		void setRadius(float r);
	private:
		PrimitiveType getType() const override final;
		size_t points{};
	};
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_CIRCLESHAPE_HPP