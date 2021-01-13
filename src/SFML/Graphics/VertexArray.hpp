#ifndef SERIOUS_PROTON_SFML_OVER_SDL_VERTEXARRAY_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_VERTEXARRAY_HPP
#include <cstdint>
#include <cstddef>

#include <array>
#include <vector>

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Drawable.hpp>
namespace sf
{
	class VertexArray : public Drawable
	{
	public:
		explicit VertexArray(PrimitiveType type, std::size_t vertexCount = 0);
		~VertexArray() override;
		Vertex& operator [](std::size_t index);
		const Vertex& operator [](std::size_t index) const;
		std::size_t getVertexCount() const;
		void draw(RenderTarget&, RenderStates) const override final;

		// non-sfml
		void setElements(std::vector<uint32_t>&& elements);
	private:
		std::vector<Vertex> vertices;
		std::vector<uint32_t> elements;
		std::array<uint32_t, 2> buffers = { 0, 0 };
		uint32_t type = 0;
		mutable bool dirty = true;
	};
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_VERTEXARRAY_HPP