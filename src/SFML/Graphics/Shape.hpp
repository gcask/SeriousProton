#ifndef SERIOUS_PROTON_SFML_OVER_SDL_SHAPE_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_SHAPE_HPP
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>

#include <array>
#include <memory>
#include <vector>
namespace sf
{
	class Texture;
	class Shape : public Drawable, public Transformable
	{
	public:
		~Shape() override;
		void setFillColor(const Color& color);
		void setOutlineColor(const Color& color);
		void setOutlineThickness(float thickness);
		void setTexture(const Texture* texture, bool resetRect = false);
		void setTextureRect(const IntRect& rect);
		const IntRect& getTextureRect() const;
		void draw(RenderTarget&, RenderStates) const override final;
	protected:
		Shape();
		void setFilledElements(const std::vector<uint32_t>&);
		void setFilledVertices(const std::vector<Vector2f>&);
		void setOutlineElements(const std::vector<uint32_t>&);
	private:
		virtual PrimitiveType getType() const = 0;
		static std::unique_ptr<Shader> filledShader;
		std::array<uint32_t, 3> buffers = { 0, 0, 0 };
		IntRect textureRect;
		Color fill = Color::White;
		Color outline = Color::White;
		const Texture* texture = nullptr;
		float outlineThickness = 0.f;
		int32_t elementCount = 0;
		int32_t outlineElementCount = 0;
	};
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_SHAPE_HPP