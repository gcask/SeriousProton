#ifndef SERIOUS_PROTON_SFML_OVER_SDL_SHAPE_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_SHAPE_HPP
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>

#include <array>
#include <memory>
#include <type_traits>
#include <initializer_list>
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
		struct VertexInfo
		{
			Vector2f position;
			Vector2f texcoords;
		};
		Shape();
		template<typename T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
		void setFilledElements(std::initializer_list<T> elements)
		{
			setFilledElements(std::begin(elements), sizeof(T), elements.size());
		}
		void setVertices(std::initializer_list<VertexInfo>);
		void setVertices(const std::vector<VertexInfo>&);

		template<typename T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
		void setOutlineElements(std::initializer_list<T> elements)
		{
			setOutlineElements(std::begin(elements), sizeof(T), elements.size());
		}

		template<typename T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
		void setFilledElements(const std::vector<T>& elements)
		{
			setFilledElements(elements.data(), sizeof(T), elements.size());
		}

		template<typename T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
		void setOutlineElements(const std::vector<T>& elements)
		{
			setOutlineElements(elements.data(), sizeof(T), elements.size());
		}
		
		void setVertices(const VertexInfo*, size_t);

		void setFilledElements(const void*, size_t typeSize, size_t elementCount);
		void setOutlineElements(const void*, size_t typeSize, size_t elementCount);
	private:
		enum class Buffer : size_t
		{
			Vertex = 0,
			ElementsFilled = 1,
			ElementsOutline = 2,

			Count
		};
		using buffer_cast = std::underlying_type_t<Buffer>;
		virtual PrimitiveType getType() const = 0;
		std::array<uint32_t, buffer_cast(Buffer::Count)> buffers{ 0 };
		IntRect textureRect;
		Color fill = Color::White;
		Color outline = Color::White;
		const Texture* texture = nullptr;
		float outlineThickness = 0.f;
		int32_t elementCount = 0;
		int32_t outlineElementCount = 0;
		uint32_t filledElementType = 0;
		uint32_t outlineElementType = 0;
	};
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_SHAPE_HPP