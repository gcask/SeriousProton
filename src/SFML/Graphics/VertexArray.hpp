#ifndef SERIOUS_PROTON_SFML_OVER_SDL_VERTEXARRAY_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_VERTEXARRAY_HPP
#include <cstdint>
#include <cstddef>
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
	private:
		std::vector<Vertex> vertices;
		uint32_t vbo = 0;
		uint32_t type = 0;
	};
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_VERTEXARRAY_HPP