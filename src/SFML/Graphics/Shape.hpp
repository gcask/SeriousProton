#ifndef SERIOUS_PROTON_SFML_OVER_SDL_SHAPE_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_SHAPE_HPP
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
namespace sf
{
	class Texture;
	class Shape : public Drawable, public Transformable
	{
	public:
		void setFillColor(const Color& color);
		void setOutlineColor(const Color& color);
		void setOutlineThickness(float thickness);
		void setTexture(const Texture* texture, bool resetRect = false);
		void setTextureRect(const IntRect& rect);
		const IntRect& getTextureRect() const;
		void draw(RenderTarget&, RenderStates) const override final;
	private:
		IntRect textureRect;
		Color fill;
		Color outline;
		const Texture* texture = nullptr;
		float outlineThickness;
	};
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_SHAPE_HPP