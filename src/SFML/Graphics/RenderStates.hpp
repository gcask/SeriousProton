#ifndef SERIOUS_PROTON_SFML_OVER_SDL_RENDERSTATES_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_RENDERSTATES_HPP
#include <SFML/Graphics/BlendMode.hpp>
#include <SFML/Graphics/Transform.hpp>
namespace sf
{
	class Shader;
	class Texture;
	class RenderStates
	{
	public:
		static const RenderStates Default; ///< Special instance holding the default render states
		RenderStates(const BlendMode& theBlendMode);
		RenderStates(const Shader*);
		RenderStates(const Texture* theTexture);

	};
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_TRANSFORMABLE_HPP