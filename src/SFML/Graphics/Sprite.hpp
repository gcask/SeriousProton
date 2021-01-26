#ifndef SERIOUS_PROTON_SFML_OVER_SDL_SPRITE_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_SPRITE_HPP
#include <cstdint>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Rect.hpp>

// Non sfml
#include <SFML/Graphics/RectangleShape.hpp>
namespace sf
{
	class Texture;
	class Sprite : public Drawable, public Transformable
	{
	public:
		Sprite();
		explicit Sprite(const Texture& texture);
		Sprite(const Texture& texture, const IntRect& rectangle);

		void setTexture(const Texture& texture, bool resetRect = false);
		void setTextureRect(const IntRect& rectangle);
		const IntRect& getTextureRect() const;
		void setColor(const Color& color);
		void draw(RenderTarget&, RenderStates) const override final;
	private:
		mutable RectangleShape impl;
	};
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_SPRITE_HPP