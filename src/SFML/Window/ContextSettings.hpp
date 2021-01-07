#ifndef SERIOUS_PROTON_SFML_OVER_SDL_CONTEXTSETTINGS_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_CONTEXTSETTINGS_HPP
#include <cstdint>
namespace sf
{
	struct ContextSettings
	{
        enum Attribute
        {
            Default = 0,      ///< Non-debug, compatibility context (this and the core attribute are mutually exclusive)
            Core = 1 << 0, ///< Core attribute
            Debug = 1 << 2  ///< Debug attribute
        };

        explicit ContextSettings(uint32_t depth = 0, uint32_t stencil = 0, uint32_t antialiasing = 0, uint32_t major = 1, uint32_t minor = 1, uint32_t attributes = Default, bool sRgb = false) :
            depthBits(depth),
            stencilBits(stencil),
            antialiasingLevel(antialiasing),
            majorVersion(major),
            minorVersion(minor),
            attributeFlags(attributes),
            sRgbCapable(sRgb)
        {
        }

        uint32_t depthBits;         ///< Bits of the depth buffer
        uint32_t stencilBits;       ///< Bits of the stencil buffer
        uint32_t antialiasingLevel; ///< Level of antialiasing
        uint32_t majorVersion;      ///< Major number of the context version to create
        uint32_t minorVersion;      ///< Minor number of the context version to create
        uint32_t   attributeFlags;    ///< The attribute flags to create the context with
        bool     sRgbCapable;       ///< Whether the context framebuffer is sRGB capable
	};
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_CONTEXTSETTINGS_HPP