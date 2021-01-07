#ifndef SERIOUS_PROTON_SFML_OVER_SDL_RENDERTARGET_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_RENDERTARGET_HPP
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/View.hpp>


#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Vector2.hpp>
namespace sf
{
	class Drawable;
	class RenderTarget : NonCopyable
	{
	public:
		virtual ~RenderTarget();
		virtual sf::Vector2u getSize() const;
		const View& getView() const;
		void setView(const View& view);
		void clear(const Color& color = Color(0, 0, 0, 255));

		void draw(const Drawable& drawable, const RenderStates& states = RenderStates::Default);
		Vector2f mapPixelToCoords(const Vector2i& point) const;
		Vector2i mapCoordsToPixel(const Vector2f& point) const;
		void popGLStates();
		void pushGLStates();
	};
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_RENDERTARGET_HPP