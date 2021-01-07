#include "BlendMode.hpp"
#include "CircleShape.hpp"
#include "Color.hpp"
#include "Drawable.hpp"
#include "Font.hpp"
#include "Glsl.hpp"
#include "Glyph.hpp"
#include "Image.hpp"
#include "Rect.hpp"
#include "RectangleShape.hpp"
#include "RenderStates.hpp"
#include "RenderTarget.hpp"
#include "RenderTexture.hpp"
#include "RenderWindow.hpp"
#include "Shader.hpp"
#include "Shape.hpp"
#include "Sprite.hpp"
#include "Text.hpp"
#include "Texture.hpp"
#include "Transform.hpp"
#include "Transformable.hpp"
#include "Vertex.hpp"
#include "View.hpp"

#include "SDL.h"
#include <glm/ext/matrix_transform.hpp>

#include <stdexcept>

namespace sf
{
    class not_implemented : public std::runtime_error
    {
    public:
        not_implemented()
            :std::runtime_error("not implemented")
        {}
    };
	// BlendMode
    BlendMode::BlendMode(Factor sourceFactor, Factor destinationFactor, Equation blendEquation)
        :BlendMode(sourceFactor, destinationFactor, blendEquation, sourceFactor, destinationFactor, blendEquation)
    {

    }

    BlendMode::BlendMode(Factor colorSourceFactor, Factor colorDestinationFactor,
        Equation colorBlendEquation, Factor alphaSourceFactor,
        Factor alphaDestinationFactor, Equation alphaBlendEquation)
        :color{colorSourceFactor, colorDestinationFactor, colorBlendEquation}
        , alpha{ alphaSourceFactor, alphaDestinationFactor, alphaBlendEquation }
    {

    }

    const BlendMode BlendAlpha(
        // color
        BlendMode::SrcAlpha, BlendMode::OneMinusSrcAlpha, BlendMode::Add,
        // alpha
        BlendMode::One, BlendMode::OneMinusSrcAlpha, BlendMode::Add
    );

    const BlendMode BlendAdd(
        // color
        BlendMode::SrcAlpha, BlendMode::One, BlendMode::Add,
        // alpha
        BlendMode::One, BlendMode::One, BlendMode::Add
    );

    const BlendMode BlendMultiply(BlendMode::DstColor, BlendMode::Zero);
    const BlendMode BlendNone(BlendMode::One, BlendMode::Zero);

    // CircleShape
    CircleShape::CircleShape(float radius, std::size_t pointCount)
    {

    }

    // Default is opaque black.
    Color::Color()
        :Color(0, 0, 0, 255)
    {

    }

    Color::Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
        : SDL_Color{ red, green, blue, alpha }
    {

    }
    // Follows SFML implementation.
    Color::Color(Uint32 color)
        : Color(
            (color & 0xff000000) >> 24,
            (color & 0x00ff0000) >> 16,
            (color & 0x0000ff00) >> 8,
            (color & 0x000000ff) >> 0)
    {
    }


    const Color Color::White(255, 255, 255);
    const Color Color::Black(0, 0, 0);
    const Color Color::Red(255, 0, 0);
    const Color Color::Green(0, 255, 0);
    const Color Color::Blue(0, 0, 255);
    const Color Color::Yellow(255, 255, 0);
    const Color Color::Magenta(255, 0, 255);
    const Color Color::Cyan(0, 255, 255);
    const Color Color::Transparent(0, 0, 0, 0);

    // Color
    bool operator ==(const Color& left, const Color& right)
    {
        return left.r == right.r
            && left.g == right.g
            && left.b == right.b
            && left.a == right.a;
    }

    bool operator !=(const Color& left, const Color& right)
    {
        return !(left == right);
    }

    Color operator -(const Color& left, const Color& right)
    {
        return Color(Uint8(std::max(int16_t(left.r) - right.r, 0)),
            Uint8(std::max(int16_t(left.g) - right.g, 0)),
            Uint8(std::max(int16_t(left.b) - right.b, 0)),
            Uint8(std::max(int16_t(left.a) - right.a, 0)));
    }

    // Drawable
    Drawable::~Drawable() = default;

    // Font
    bool Font::loadFromStream(InputStream& stream)
    {
        throw not_implemented();
    }
    float Font::getLineSpacing(uint32_t characterSize) const
    {
        throw not_implemented();
        return 0.f;
    }

    const Glyph& Font::getGlyph(Uint32 codePoint, uint32_t characterSize, bool bold, float outlineThickness) const
    {
        throw not_implemented();
    }

    // Glsl
    namespace Glsl
    {
        Vec4::Vec4(const Color&)
        {
            throw not_implemented();
        }
    }

    // Image
    void Image::create(unsigned int width, unsigned int height, const Color& color)
    {
        throw not_implemented();
    }
    bool Image::loadFromImage(const Image& image, const IntRect& area)
    {
        throw not_implemented();
    }
    Vector2u Image::getSize() const
    {
        throw not_implemented();
    }
    bool Image::loadFromStream(InputStream& stream)
    {
        throw not_implemented();
    }
    Color Image::getPixel(unsigned int x, unsigned int y) const
    {
        throw not_implemented();
    }

    // RectangleShape
    RectangleShape::RectangleShape(const Vector2f& size)
    {
        throw not_implemented();
    }
    void RectangleShape::setSize(const Vector2f& size)
    {
        throw not_implemented();
    }
    const Vector2f& RectangleShape::getSize() const
    {
        throw not_implemented();
    }

    // RenderStates
    const RenderStates RenderStates::Default(BlendAlpha); ///< Special instance holding the default render states
    RenderStates::RenderStates(const BlendMode& theBlendMode)
    {
        throw not_implemented();
    }
    RenderStates::RenderStates(const Shader*)
    {
        throw not_implemented();
    }
    RenderStates::RenderStates(const Texture* theTexture)
    {
        throw not_implemented();
    }

    // RenderTarget
    RenderTarget::~RenderTarget()
    {
        throw not_implemented();
    }
    sf::Vector2u RenderTarget::getSize() const
    {
        throw not_implemented();
    }
    const View& RenderTarget::getView() const
    {
        throw not_implemented();
    }
    void RenderTarget::setView(const View& view)
    {
        throw not_implemented();
    }
    void RenderTarget::clear(const Color& color)
    {
        throw not_implemented();
    }

    void RenderTarget::draw(const Drawable& drawable, const RenderStates& states)
    {
        throw not_implemented();
    }
    Vector2f RenderTarget::mapPixelToCoords(const Vector2i& point) const
    {
        throw not_implemented();
    }
    Vector2i RenderTarget::mapCoordsToPixel(const Vector2f& point) const
    {
        throw not_implemented();
    }
    void RenderTarget::popGLStates()
    {
        throw not_implemented();
    }
    void RenderTarget::pushGLStates()
    {
        throw not_implemented();
    }

    // RenderTexture
    bool RenderTexture::create(unsigned int width, unsigned int height, bool depthBuffer)
    {
        throw not_implemented();
    }
    void RenderTexture::setRepeated(bool repeated)
    {
        throw not_implemented();
    }
    void RenderTexture::setSmooth(bool smooth)
    {
        throw not_implemented();
    }
    void RenderTexture::display()
    {
        throw not_implemented();
    }
    const Texture& RenderTexture::getTexture() const
    {
        throw not_implemented();
    }

    // RenderWindow
    struct RenderWindow::Impl
    {

    };
    RenderWindow::RenderWindow()
    {
        throw not_implemented();
    }
    RenderWindow::~RenderWindow()
    {
        throw not_implemented();
    }
    void RenderWindow::setFramerateLimit(int32_t)
    {
        throw not_implemented();
    }
    void RenderWindow::setTitle(const std::string&)
    {
        throw not_implemented();
    }
    bool RenderWindow::isOpen() const
    {
        throw not_implemented();
    }
    const sf::View& RenderWindow::getView() const
    {
        throw not_implemented();
    }
    sf::Vector2u RenderWindow::getSize() const
    {
        throw not_implemented();
    }
    void* RenderWindow::getSystemHandle() const
    {
        throw not_implemented();
    }
    void RenderWindow::display()
    {
        throw not_implemented();
    }
    void RenderWindow::close()
    {
        throw not_implemented();
    }

    void RenderWindow::create(VideoMode mode, const std::string& title, Uint32 style, const ContextSettings& settings)
    {
        throw not_implemented();
    }
    const ContextSettings& RenderWindow::getSettings() const
    {
        throw not_implemented();
    }
    void RenderWindow::setVerticalSyncEnabled(bool)
    {
        throw not_implemented();
    }
    void RenderWindow::setMouseCursorVisible(bool)
    {
        throw not_implemented();
    }

    // Shader
    bool Shader::isAvailable()
    {
        throw not_implemented();
    }
    void Shader::bind(const Shader* shader)
    {
        throw not_implemented();
    }
    bool Shader::loadFromFile(const std::string&, Type)
    {
        throw not_implemented();
    }
    bool Shader::loadFromStream(InputStream& vertexShaderStream, InputStream& fragmentShaderStream)
    {
        throw not_implemented();
    }

    // Shape
    void Shape::setFillColor(const Color& color)
    {
        throw not_implemented();
    }
    void Shape::setOutlineColor(const Color& color)
    {
        throw not_implemented();
    }
    void Shape::setOutlineThickness(float thickness)
    {
        throw not_implemented();
    }
    void Shape::setTexture(const Texture* texture, bool resetRect)
    {
        throw not_implemented();
    }
    void Shape::setTextureRect(const IntRect& rect)
    {
        throw not_implemented();
    }
    const IntRect& Shape::getTextureRect() const
    {
        throw not_implemented();
    }

    // Sprite
    Sprite::Sprite()
    {
        throw not_implemented();
    }
    Sprite::Sprite(const Texture& texture)
    {
        throw not_implemented();
    }
    Sprite::Sprite(const Texture& texture, const IntRect& rectangle)
    {
        throw not_implemented();
    }

    void Sprite::setTexture(const Texture& texture, bool resetRect)
    {
        throw not_implemented();
    }
    void Sprite::setTextureRect(const IntRect& rectangle)
    {
        throw not_implemented();
    }
    const IntRect& Sprite::getTextureRect() const
    {
        throw not_implemented();
    }
    void Sprite::setColor(const Color& color)
    {
        throw not_implemented();
    }

    // Text
    Text::Text(const String& string, const Font& font, unsigned int characterSize)
    {
        throw not_implemented();
    }
    void Text::setColor(const Color& color)
    {
        throw not_implemented();
    }
    FloatRect Text::getLocalBounds() const
    {
        throw not_implemented();
    }

    // Texture
    void Texture::bind(const Texture* texture, CoordinateType coordinateType)
    {
        throw not_implemented();
    }
    Texture::~Texture()
    {
        throw not_implemented();
    }
    Vector2u Texture::getSize() const
    {
        throw not_implemented();
    }
    bool Texture::loadFromImage(const Image& image, const IntRect& area)
    {
        throw not_implemented();
    }
    void Texture::setRepeated(bool repeated)
    {
        throw not_implemented();
    }
    void Texture::setSmooth(bool smooth)
    {
        throw not_implemented();
    }

    // Transform
    Transform::Transform()
        :matrix(glm::identity<glm::mat4>())
    {
    }
    Transform::Transform(float a00, float a01, float a02,
        float a10, float a11, float a12,
        float a20, float a21, float a22)
    {
        throw not_implemented();
    }
    Vector2f Transform::transformPoint(const Vector2f& point) const
    {
        throw not_implemented();
    }

    // Transformable
    Transformable::~Transformable()
    {
        throw not_implemented();
    }
    void Transformable::setScale(float factorX, float factorY)
    {
        throw not_implemented();
    }
    void Transformable::setOrigin(float x, float y)
    {
        throw not_implemented();
    }
    void Transformable::setOrigin(const Vector2f& position)
    {
        throw not_implemented();
    }
    void Transformable::setPosition(float x, float y)
    {
        throw not_implemented();
    }
    void Transformable::setPosition(const Vector2f& position)
    {
        throw not_implemented();
    }
    void Transformable::setRotation(float angle)
    {
        throw not_implemented();
    }

    // Vertex
    Vertex::Vertex()
    {
        throw not_implemented();
    }

    // VertexArray
    VertexArray::VertexArray(PrimitiveType type, std::size_t vertexCount)
    {
        throw not_implemented();
    }
    Vertex& VertexArray::operator [](std::size_t index)
    {
        throw not_implemented();
    }
    const Vertex& VertexArray::operator [](std::size_t index) const
    {
        throw not_implemented();
    }
    std::size_t VertexArray::getVertexCount() const
    {
        throw not_implemented();
    }

    // View
    View::View()
    {
        throw not_implemented();
    }
    View::View(const Vector2f& center, const Vector2f& size)
    {
        throw not_implemented();
    }
    View::View(const FloatRect& rectangle)
    {
        throw not_implemented();
    }
    void View::setViewport(const FloatRect& viewport)
    {
        throw not_implemented();
    }
    const FloatRect& View::getViewport() const
    {
        throw not_implemented();
    }
    const Vector2f& View::getSize() const
    {
        throw not_implemented();
    }
}