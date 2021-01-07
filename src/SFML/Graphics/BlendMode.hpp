#ifndef SERIOUS_PROTON_SFML_OVER_SDL_BLENDMODE_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_BLENDMODE_HPP
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

namespace sf
{
	struct BlendMode
	{
        enum Factor
        {
            Zero,             ///< (0, 0, 0, 0)
            One,              ///< (1, 1, 1, 1)
            SrcColor,         ///< (src.r, src.g, src.b, src.a)
            OneMinusSrcColor, ///< (1, 1, 1, 1) - (src.r, src.g, src.b, src.a)
            DstColor,         ///< (dst.r, dst.g, dst.b, dst.a)
            OneMinusDstColor, ///< (1, 1, 1, 1) - (dst.r, dst.g, dst.b, dst.a)
            SrcAlpha,         ///< (src.a, src.a, src.a, src.a)
            OneMinusSrcAlpha, ///< (1, 1, 1, 1) - (src.a, src.a, src.a, src.a)
            DstAlpha,         ///< (dst.a, dst.a, dst.a, dst.a)
            OneMinusDstAlpha,  ///< (1, 1, 1, 1) - (dst.a, dst.a, dst.a, dst.a)
            //
            Count
        };

        enum Equation
        {
            Add,
            Subtract,
            ReverseSubtract
        };

        BlendMode(Factor sourceFactor, Factor destinationFactor, Equation blendEquation = Add);
        BlendMode(Factor colorSourceFactor, Factor colorDestinationFactor,
            Equation colorBlendEquation, Factor alphaSourceFactor,
            Factor alphaDestinationFactor, Equation alphaBlendEquation);
        struct BlendDetails
        {
            Factor src;
            Factor dst;
            Equation equation;
        } color, alpha;
	};

    extern const BlendMode BlendAlpha;
    extern const BlendMode BlendAdd;
    extern const BlendMode BlendMultiply;
    extern const BlendMode BlendNone;
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_BLENDMODE_HPP