#ifndef SERIOUS_PROTON_SFML_OVER_SDL_TEXT_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_TEXT_HPP
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/String.hpp>

#include <array>
#include <SFML/Graphics/RectangleShape.hpp>
#include <array>
namespace sf
{
	class Text : public Drawable, public Transformable
	{
    public:
        enum Style
        {
            Regular = 0,      ///< Regular characters, no style
            Bold = 1 << 0, ///< Bold characters
            Italic = 1 << 1, ///< Italic characters
            Underlined = 1 << 2, ///< Underlined characters
            StrikeThrough = 1 << 3  ///< Strike through characters
        };

        Text(const String& string, const Font& font, unsigned int characterSize = 30);
        Text(const Text&) = delete;
        Text& operator =(const Text&) = delete;
        Text(Text&&) = delete;
        Text& operator =(Text&&) = delete;
        ~Text() override;
        void setColor(const Color& color);
        FloatRect getLocalBounds() const;
        void draw(RenderTarget&, RenderStates) const override final;
    private:
        struct VertexInfo
        {
            Vector2f position;
            Vector2f texcoords;
        };
        Vector2f extents;
        const Texture *texture = nullptr;
        std::array<uint32_t, 2> buffers{ 0, 0 };
        size_t textLength = 0;
        Color fill = Color::White;
	};
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_TEXT_HPP