#ifndef SERIOUS_PROTON_SFML_OVER_SDL_BLENDMODE_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_BLENDMODE_HPP
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include "SDL_video.h"
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

        static constexpr SDL_BlendFactor factorAsSDL(Factor factor)
        {
            switch (factor)
            {
            case Zero:
                return SDL_BLENDFACTOR_ZERO;
            case One:
                return SDL_BLENDFACTOR_ONE;
            case SrcColor:
                return SDL_BLENDFACTOR_SRC_COLOR;
            case OneMinusSrcColor:
                return SDL_BLENDFACTOR_ONE_MINUS_SRC_COLOR;
            case DstColor:
                return SDL_BLENDFACTOR_DST_COLOR;
            case OneMinusDstColor:
                return SDL_BLENDFACTOR_ONE_MINUS_DST_COLOR;
            case SrcAlpha:
                return SDL_BLENDFACTOR_SRC_ALPHA;
            case OneMinusSrcAlpha:
                return SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
            case DstAlpha:
                return SDL_BLENDFACTOR_DST_ALPHA;
            case OneMinusDstAlpha:
                return SDL_BLENDFACTOR_ONE_MINUS_DST_ALPHA;
            }

            return SDL_BLENDFACTOR_ZERO;
        }

        enum Equation
        {
            Add,
            Subtract,
            ReverseSubtract
        };

        static constexpr SDL_BlendOperation equationAsSDL(Equation equation)
        {
            switch (equation)
            {
            case Add:
                return SDL_BLENDOPERATION_ADD;
            case Subtract:
                return SDL_BLENDOPERATION_SUBTRACT;
            case ReverseSubtract:
                return SDL_BLENDOPERATION_REV_SUBTRACT;
            }

            return SDL_BLENDOPERATION_ADD;
        }

        BlendMode(Factor sourceFactor, Factor destinationFactor, Equation blendEquation = Add);
        BlendMode(Factor colorSourceFactor, Factor colorDestinationFactor,
            Equation colorBlendEquation, Factor alphaSourceFactor,
            Factor alphaDestinationFactor, Equation alphaBlendEquation);
        BlendMode();
        SDL_BlendMode sdlObject;
	};

    extern const BlendMode BlendAlpha;
    extern const BlendMode BlendAdd;
    extern const BlendMode BlendMultiply;
    extern const BlendMode BlendNone;
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_BLENDMODE_HPP