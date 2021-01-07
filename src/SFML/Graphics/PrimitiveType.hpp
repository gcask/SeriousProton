#ifndef SERIOUS_PROTON_SFML_OVER_SDL_PRIMITIVETYPE_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_PRIMITIVETYPE_HPP
#include <cstdint>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Rect.hpp>
namespace sf
{
    enum PrimitiveType
    {
        Points,        ///< List of individual points
        Lines,         ///< List of individual lines
        LineStrip,     ///< List of connected lines, a point uses the previous point to form a line
        Triangles,     ///< List of individual triangles
        TriangleStrip, ///< List of connected triangles, a point uses the two previous points to form a triangle
        TriangleFan,   ///< List of connected triangles, a point uses the common center and the previous point to form a triangle
        Quads,         ///< List of individual quads (deprecated, don't work with OpenGL ES)

        // Deprecated names
        LinesStrip = LineStrip,     ///< \deprecated Use LineStrip instead
        TrianglesStrip = TriangleStrip, ///< \deprecated Use TriangleStrip instead
        TrianglesFan = TriangleFan    ///< \deprecated Use TriangleFan instead
    };
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_SPRITE_HPP