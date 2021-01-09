#ifndef SERIOUS_PROTON_SFML_OVER_SDL_VIEW_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_VIEW_HPP
#include <cstdint>
#include <SFML/Graphics/Rect.hpp>
namespace sf
{
	class View
	{
	public:
		View();
		View(const Vector2f& center, const Vector2f& size);
		explicit View(const FloatRect& rectangle);
		void setViewport(const FloatRect& viewport);
		const FloatRect& getViewport() const;
		const Vector2f& getSize() const;
	private:
		Vector2f center;
		Vector2f size;
		FloatRect viewport;
	};
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_VIEW_HPP