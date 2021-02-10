#ifndef SERIOUS_PROTON_SFML_OVER_SDL_RENDERTEXTURE_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_RENDERTEXTURE_HPP
#include <cstdint>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/ContextSettings.hpp>
namespace sf
{
	class RenderTexture : public RenderTarget
	{
	public:
		RenderTexture();
		~RenderTexture();
		bool create(unsigned int width, unsigned int height, bool depthBuffer);
		void setRepeated(bool repeated);
		void setSmooth(bool smooth);
		void display();
		const Texture& getTexture() const;
		sf::Vector2u getSize() const override final;
	private:
		uint32_t rbo = 0;
		Texture texture;
	};
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_RENDERTEXTURE_HPP