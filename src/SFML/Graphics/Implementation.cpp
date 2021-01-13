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
#include "SDL_image.h"
#include "SDL_ttf.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "GL/glew.h"

#include <stdexcept>
#include <mutex>
#include <chrono>

#include <SFML/System/MemoryInputStream.hpp>

namespace std
{
    template<>
    class lock_guard<SDL_Surface>
    {
    public:
        lock_guard(SDL_Surface& theSurface)
            :surface(&theSurface)
        {
            SDL_LockSurface(surface);
        }

        lock_guard(SDL_Surface& theSurface, adopt_lock_t)
            :surface(&theSurface)
        {

        }

        ~lock_guard()
        {
            SDL_UnlockSurface(surface);
        }
    private:
        SDL_Surface* surface;
    };
}

#ifndef CHECKED_GL
#ifndef NDEBUG
#define CHECKED_GL 1
#else
#define CHECKED_GL 0
#endif
#endif

#if CHECKED_GL
#define glChecked(call) \
    do { \
        auto error = glGetError(); \
        SDL_assert(error == GL_NO_ERROR); \
        call; \
        error = glGetError(); \
        SDL_assert(error == GL_NO_ERROR); \
    } while(false)
#else
#define glChecked(call) call
#endif
namespace sf
{
    namespace
    {
        Shader DefaultShader;
        struct ScopedShader final
        {
            explicit ScopedShader(Shader& shader)
            {
                Shader::bind(&shader);
            }

            ~ScopedShader()
            {
                Shader::bind(nullptr);
            }
        };

        namespace gl
        {
            GLenum primitive_cast(PrimitiveType type)
            {
                switch (type)
                {
                case Points:
                    return GL_POINTS;
                case Lines:
                    return GL_LINES;
                case LineStrip:
                    return GL_LINE_STRIP;
                case Triangles:
                    return GL_TRIANGLES;
                case TriangleStrip:
                    return GL_TRIANGLE_STRIP;
                case TriangleFan:
                    return GL_TRIANGLE_FAN;
                case Quads:
                    return GL_QUADS;
                }

                return GL_NONE;
            }
        }

        constexpr const char* vertexShader = R"glsl(#version 100
attribute vec2 position;
attribute vec4 color;
uniform mat4 projection;
varying vec4 vertexColor;
void main()
{
    gl_Position = projection * vec4(position, 0.0, 1.0);
    vertexColor = color;
}
)glsl";

        constexpr const char * fragmentShader = R"glsl(#version 100
precision mediump float;
varying vec4 vertexColor;
void main()
{
    gl_FragColor = vertexColor;
}
)glsl";

        constexpr const char shapeVertexShader[] = R"glsl(#version 100
attribute vec2 position;
uniform mat4 projection;
void main()
{
    gl_Position = projection * vec4(position, 0.0, 1.0);
}
)glsl";

        constexpr const char shapeFragmentShader[] = R"glsl(#version 100
precision mediump float;
uniform vec4 fillColor;
void main()
{
    gl_FragColor = fillColor;
}
)glsl";
    }
    namespace rwops
    {
        InputStream& getStream(SDL_RWops* context)
        {
            return *static_cast<InputStream*>(context->hidden.unknown.data1);
        }
        Sint64 size(SDL_RWops* context)
        {
            auto& stream = getStream(context);
            return stream.getSize();
        }

        Sint64 seek(SDL_RWops* context, Sint64 offset, int whence)
        {
            auto& stream = getStream(context);
            switch (whence)
            {
            case RW_SEEK_SET:
                break;
            case RW_SEEK_CUR:
                offset += stream.tell();
                break;
            case RW_SEEK_END:
                offset += stream.getSize();
                break;
            }

            return stream.seek(offset);
        }

        size_t read(SDL_RWops* context, void* ptr, size_t size, size_t maxnum)
        {
            auto& stream = getStream(context);
            auto result = stream.read(ptr, size * maxnum);
            return result < 0 ? 0 : static_cast<size_t>(result);
        }

        int close(SDL_RWops* context)
        {
            context->hidden.unknown.data1 = nullptr;
            SDL_FreeRW(context);
            return 0;
        }

        SDL_RWops* fromStream(InputStream& stream)
        {
            auto ops = SDL_AllocRW();
            if (!ops)
                return nullptr;
            ops->size = &size;
            ops->seek = &seek;
            ops->read = &read;
            ops->write = nullptr;
            ops->close = &close;
            ops->type = SDL_RWOPS_UNKNOWN;
            ops->hidden.unknown.data1 = &stream;
            return ops;
        }
    }
    class not_implemented : public std::runtime_error
    {
    public:
        not_implemented()
            :std::runtime_error("not implemented")
        {}
    };
#pragma region BlendMode
	// BlendMode
    BlendMode::BlendMode(Factor sourceFactor, Factor destinationFactor, Equation blendEquation)
        :BlendMode(sourceFactor, destinationFactor, blendEquation, sourceFactor, destinationFactor, blendEquation)
    {

    }

    BlendMode::BlendMode(Factor colorSourceFactor, Factor colorDestinationFactor,
        Equation colorBlendEquation, Factor alphaSourceFactor,
        Factor alphaDestinationFactor, Equation alphaBlendEquation)
        : sdlObject(SDL_ComposeCustomBlendMode(
            factorAsSDL(colorSourceFactor), factorAsSDL(colorDestinationFactor), equationAsSDL(colorBlendEquation),
            factorAsSDL(alphaSourceFactor), factorAsSDL(alphaDestinationFactor), equationAsSDL(alphaBlendEquation)
        ))
    {

    }

    BlendMode::BlendMode()
        :sdlObject(SDL_BLENDMODE_BLEND)
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
#pragma endregion BlendMode
#pragma region CircleShape
    CircleShape::CircleShape(float radius, std::size_t pointCount)
    {
        std::vector<uint32_t> elements(pointCount + 2);
        std::vector<Vector2f> vertices(pointCount + 1); // points + center
        // First vertex is the center.
        vertices[0] = { radius, radius };
        elements[0] = 0;
        for (size_t i = 1; i < pointCount + 1; ++i)
        {
            float angle = i * 2 * M_PI / pointCount - M_PI / 2;
            float x = std::cos(angle) * radius;
            float y = std::sin(angle) * radius;
            vertices[i] = { radius + x, radius + y };
            elements[i] = i;
        }
        elements[pointCount + 1] = 1;
        setFilledElements(elements);
        setFilledVertices(vertices);
        // For the outline, we want to "loop back" to the first (or last) vertex.
        elements.pop_back();
        elements.front() = elements.back();
        setOutlineElements(elements);
    }

    PrimitiveType CircleShape::getType() const
    {
        return TriangleFan;
    }
#pragma endregion CircleShape
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


#pragma region Font
    bool Font::loadFromStream(InputStream& stream)
    {
        if (!TTF_WasInit())
        {
            if (TTF_Init() < 0)
                return false;
        }
        if (sdlObject)
        {
            TTF_CloseFont(sdlObject);
            sdlObject = nullptr;
        }
            
        sdlObject = TTF_OpenFontRW(rwops::fromStream(stream), SDL_TRUE, 16);
        return sdlObject != nullptr;
    }
    float Font::getLineSpacing(uint32_t characterSize) const
    {
        SDL_assert(sdlObject);
        return TTF_FontLineSkip(sdlObject);
    }

    const Glyph& Font::getGlyph(Uint32 codePoint, uint32_t characterSize, bool bold, float outlineThickness) const
    {
        throw not_implemented();
    }
#pragma endregion Font

#pragma region Glsl
    namespace Glsl
    {
        Vec4::Vec4(const Color&)
        {
            throw not_implemented();
        }
    }
#pragma endregion Glsl

#pragma region Image
    namespace
    {
        void ensureImgLoaded()
        {
            static constexpr auto desired = IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP;
            auto available = IMG_Init(desired);
            if ((available & desired) != desired)
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "IMG_Init: Initialization issues: %s!\n", IMG_GetError());
            }
        }
    }
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
        if (sdlObject)
            return Vector2u(sdlObject->w, sdlObject->h);

        return Vector2u(0u, 0u);
    }
    bool Image::loadFromStream(InputStream& stream)
    {
        ensureImgLoaded();
        if (sdlObject)
        {
            SDL_FreeSurface(sdlObject);
            sdlObject = nullptr;
        }
        auto raw = IMG_Load_RW(rwops::fromStream(stream), 1);
        if (raw)
        {
            auto targetFormat = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
            sdlObject = SDL_ConvertSurface(raw, targetFormat, 0);
            SDL_FreeFormat(targetFormat);
            SDL_FreeSurface(raw);
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Failed to load image: %s", IMG_GetError());
        }
        return sdlObject != nullptr;
    }
    Color Image::getPixel(unsigned int x, unsigned int y) const
    {
        SDL_assert(sdlObject != nullptr);
        const std::lock_guard<SDL_Surface> guard(*sdlObject);
        Color result;
        auto pixel = static_cast<uint8_t*>(sdlObject->pixels) + x + y * sdlObject->pitch;
        return Color(pixel[0], pixel[1], pixel[2], pixel[3]);
    }
#pragma endregion Image

#pragma region RectangleShape
    RectangleShape::RectangleShape(const Vector2f& size)
    {
        setSize(size);
        setFilledElements({ 0, 1, 2, 2, 3, 0 });
        setOutlineElements({ 0, 1, 2, 3, 0 });
    }

    void RectangleShape::setSize(const Vector2f& size)
    {
        this->size = size;
        setFilledVertices({ {0, 0}, {size.x, 0}, size, {0, size.y} });
    }
    const Vector2f& RectangleShape::getSize() const
    {
        return size;
    }
    PrimitiveType RectangleShape::getType() const
    {
        return Triangles;
    }
#pragma endregion RectangleShape

    // RenderStates
    const RenderStates RenderStates::Default(BlendAlpha); ///< Special instance holding the default render states
    RenderStates::RenderStates(const BlendMode& theBlendMode)
        :blend(theBlendMode)
    {
    }
    RenderStates::RenderStates(const Shader*)
    {
        throw not_implemented();
    }
    RenderStates::RenderStates(const Texture* theTexture)
    {
        throw not_implemented();
    }

#pragma region RenderTarget
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
        return view;
    }
    void RenderTarget::setView(const View& theView)
    {
        view = theView;
    }
    void RenderTarget::clear(const Color& color)
    {
        glClearColor(color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void RenderTarget::draw(const Drawable& drawable, const RenderStates& states)
    {
        drawable.draw(*this, states);
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
#pragma endregion RenderTarget

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

#pragma region RenderWindow
    struct RenderWindow::Impl final
    {
        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;
        SDL_GLContext context = nullptr;
        ContextSettings settings;
        uint32_t delay = 0;
        uint32_t lastStart = 0;

        ~Impl()
        {
            if (context)
            {
                SDL_GL_DeleteContext(context);
            }
            if (renderer)
            {
                SDL_DestroyRenderer(renderer);
            }

            if (window)
            {
                SDL_DestroyWindow(window);
            }
        }
    };
    RenderWindow::RenderWindow()
        :impl(std::make_unique<Impl>())
    {
    }
    RenderWindow::~RenderWindow() = default;
    void RenderWindow::setFramerateLimit(int32_t hz)
    {
        SDL_assert(impl);
        impl->delay = 0;
        if (hz > 0)
            impl->delay = uint32_t((1.f / hz) * 1000);

    }
    void RenderWindow::setTitle(const std::string&title)
    {
        SDL_assert(impl && impl->window);
        SDL_SetWindowTitle(impl->window, title.c_str());
    }
    bool RenderWindow::isOpen() const
    {
        return impl && impl->window;
    }
    sf::Vector2u RenderWindow::getSize() const
    {
        SDL_assert(impl && impl->renderer);
        int w = 0, h = 0;
        if (!SDL_GetRendererOutputSize(impl->renderer, &w, &h))
            return sf::Vector2u(w, h);
        return sf::Vector2u(0u, 0u);
    }
    void* RenderWindow::getSystemHandle() const
    {
        throw not_implemented();
    }
    void RenderWindow::display()
    {
        if (impl)
        {
            auto size = getSize();
            auto viewport = getView().getViewport();
            // OpenGL viewport has (0,0) at center.
            IntRect viewportGl(
                static_cast<int32_t>(0.5f + size.x * viewport.left),
                static_cast<int32_t>(0.5f + size.y * viewport.top),
                static_cast<int32_t>(0.5f + size.x * viewport.width),
                static_cast<int32_t>(0.5f + size.y * viewport.height)
            );
            // flip "vertical" y axis.
            auto top = size.y - (viewportGl.top + viewportGl.height);
            glChecked(glViewport(viewportGl.left, top, viewportGl.width, viewportGl.height));
            SDL_assert(impl->renderer);
            SDL_GL_SwapWindow(impl->window);
            if (impl->lastStart)
            {
                // Do we need to throttle?
                auto elapsed = SDL_GetTicks() - impl->lastStart;
                if (elapsed < impl->delay)
                    SDL_Delay(impl->delay - elapsed);
            }
            impl->lastStart = SDL_GetTicks();
        }
    }
    void RenderWindow::close()
    {
        impl.reset();
    }

    void RenderWindow::create(VideoMode mode, const std::string& title, Uint32 style, const ContextSettings& settings)
    {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, settings.majorVersion);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, settings.minorVersion);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, settings.depthBits);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, settings.stencilBits);
        uint32_t flags = SDL_WINDOW_OPENGL;
        if (!(style & Style::Titlebar))
        {
            flags |= SDL_WINDOW_BORDERLESS;
        }

        if ((style & Style::Resize))
        {
            flags |= SDL_WINDOW_RESIZABLE;
        }

        if ((style & Style::Fullscreen))
        {
            flags |= SDL_WINDOW_FULLSCREEN;
        }

        if (SDL_CreateWindowAndRenderer(mode.width, mode.height, flags, &impl->window, &impl->renderer))
        {
            SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Failed to create window: %s", SDL_GetError());
        }

        impl->context = SDL_GL_CreateContext(impl->window);
        impl->settings = settings;
        int value = 0;
        if (!SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &value))
            impl->settings.stencilBits = value;
        if (!SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &value))
            impl->settings.depthBits = value;
        if (!SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &value))
            impl->settings.minorVersion = value;
        if (!SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &value))
            impl->settings.majorVersion = value;
        glewInit();
        sf::MemoryInputStream vertexShaderStream, fragmentShaderStream;
        vertexShaderStream.open(vertexShader, strlen(vertexShader) + 1);
        fragmentShaderStream.open(fragmentShader, strlen(fragmentShader) + 1);
        DefaultShader.loadFromStream(vertexShaderStream, fragmentShaderStream);
        
        setTitle(title);
    }
    const ContextSettings& RenderWindow::getSettings() const
    {
        SDL_assert(impl);
        return impl->settings;
    }
    void RenderWindow::setVerticalSyncEnabled(bool on)
    {
        SDL_SetHint(SDL_HINT_RENDER_VSYNC, on ? "1" : "0");
    }
    void RenderWindow::setMouseCursorVisible(bool visible)
    {
        SDL_ShowCursor(visible ? SDL_ENABLE : SDL_DISABLE);
    }
#pragma endregion RenderWindow
#pragma region Shader
    bool Shader::isAvailable()
    {
        return SDL_GL_GetCurrentContext() != nullptr;
    }
    void Shader::bind(const Shader* shader)
    {
        auto program = 0;
        if (shader)
        {
            SDL_assert(shader->program != 0);
            program = shader->program;
        }


        glUseProgram(program);
    }
    bool Shader::loadFromFile(const std::string&, Type)
    {
        throw not_implemented();
    }

    namespace
    {
        class ScopedRWops final
        {
        public:
            explicit ScopedRWops(SDL_RWops *ops)
                :ops(ops)
            {}
            ~ScopedRWops()
            {
                SDL_FreeRW(ops);
            }

            SDL_RWops* get() const
            {
                return ops;
            }

            operator bool() const
            {
                return ops != nullptr;
            }
        private:
            SDL_RWops* ops = nullptr;
        };
        bool compileShader(GLuint shader, InputStream& shaderSource)
        {
            ScopedRWops sdlStream(rwops::fromStream(shaderSource));
            if (!sdlStream)
                return false;

            auto length = SDL_RWsize(sdlStream.get());
            if (length < 0)
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to get stream size: %s", SDL_GetError());
                return false;
            }
            {
                std::vector<char> code(length);
                if (SDL_RWread(sdlStream.get(), code.data(), code.size(), 1) == 0)
                {
                    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Empty or error shader stream: %s", SDL_GetError());
                    return false;
                }

                const char* sources[1] = { code.data() };
                glChecked(glShaderSource(shader, 1, sources, nullptr));
            }
            glChecked(glCompileShader(shader));
            GLint status = GL_FALSE;
            glChecked(glGetShaderiv(shader, GL_COMPILE_STATUS, &status));
            SDL_assert(status == GL_TRUE);
            if (status == GL_FALSE)
            {
                GLsizei logLength = 0;
                glChecked(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength));
                std::vector<char> compileLog(size_t(logLength) + 1);
                glChecked(glGetShaderInfoLog(shader, compileLog.size(), nullptr, compileLog.data()));
                compileLog[logLength] = '\0';
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Shader compilation failed: %s", compileLog.data());
            }

            return status == GL_TRUE;
        }
    }
    bool Shader::loadFromStream(InputStream& vertexShaderStream, InputStream& fragmentShaderStream)
    {
        SDL_assert(program == 0);
        glChecked(vertexShader = glCreateShader(GL_VERTEX_SHADER));
        if (!compileShader(vertexShader, vertexShaderStream))
        {
            glChecked(glDeleteShader(vertexShader));
            vertexShader = 0;
            return false;
        }

        glChecked(fragmentShader = glCreateShader(GL_FRAGMENT_SHADER));
        if (!compileShader(fragmentShader, fragmentShaderStream))
        {
            glChecked(glDeleteShader(fragmentShader));
            fragmentShader = 0;
            glChecked(glDeleteShader(vertexShader));
            vertexShader = 0;
            return false;
        }

        glChecked(program = glCreateProgram());
        glChecked(glAttachShader(program, vertexShader));
        glChecked(glAttachShader(program, fragmentShader));
        glChecked(glLinkProgram(program));

        return true;
    }
    int32_t Shader::attribute(const char* name) const
    {
        if (!program)
            return -1;
        return glGetAttribLocation(program, name);
    }

    template<>
    void Shader::setUniform(const std::string& name, const glm::mat4&matrix)
    {
        GLint location = 0;
        glChecked(location = glGetUniformLocation(program, name.c_str()));
        glChecked(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix)));
    }

    template<>
    void Shader::setUniform(const std::string& name, const Color& color)
    {
        GLint location = 0;
        glChecked(location = glGetUniformLocation(program, name.c_str()));
        glChecked(glUniform4f(location, color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f));
    }
#pragma endregion Shader
#pragma region Shape
    std::unique_ptr<Shader> Shape::filledShader;
    Shape::~Shape()
    {
        glChecked(glDeleteBuffers(buffers.size(), buffers.data()));
    }
    void Shape::setFillColor(const Color& color)
    {
        fill = color;
    }
    void Shape::setOutlineColor(const Color& color)
    {
        outline = color;
    }
    void Shape::setOutlineThickness(float thickness)
    {
        outlineThickness = thickness;
    }
    void Shape::setTexture(const Texture* texture, bool resetRect)
    {
        // lines up with SFML current implementation.
        resetRect |= (!this->texture && textureRect == IntRect());
        if (texture && resetRect)
        {
            setTextureRect(IntRect(0, 0, texture->getSize().x, texture->getSize().y));
        }
        this->texture = texture;
    }
    void Shape::setTextureRect(const IntRect& rect)
    {
        textureRect = rect;
    }
    const IntRect& Shape::getTextureRect() const
    {
        return textureRect;
    }

    void Shape::draw(RenderTarget& target, RenderStates states) const
    {
        if (!filledShader)
        {
            filledShader = std::make_unique<Shader>();
            sf::MemoryInputStream vertexShaderStream, fragmentShaderStream;
            vertexShaderStream.open(shapeVertexShader, strlen(shapeVertexShader) + 1);
            fragmentShaderStream.open(shapeFragmentShader, strlen(shapeFragmentShader) + 1);
            filledShader->loadFromStream(vertexShaderStream, fragmentShaderStream);
        }

        glm::mat4 projection = glm::ortho(0.f, float(target.getSize().x), float(target.getSize().y), 0.f, -1.f, 1.f);
        glm::mat4 view = glm::identity<glm::mat4>();

        glChecked(glBindBuffer(GL_ARRAY_BUFFER, buffers[0]));

        // Filled shape.
        if (elementCount > 0)
        {
            glChecked(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]));
            ScopedShader guard(*filledShader);
            auto posAttrib = filledShader->attribute("position");
            filledShader->setUniform("projection", projection * view);
            filledShader->setUniform("fillColor", fill);
            glChecked(glEnableVertexAttribArray(posAttrib));
            glChecked(glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2f), (GLvoid*)0));
            glChecked(glDrawElements(gl::primitive_cast(getType()), elementCount, GL_UNSIGNED_INT, (GLvoid*)0));
            // outline
            if (outlineElementCount > 0)
            {
                // setup state.
                auto smoothed = false;
                glChecked(smoothed = glIsEnabled(GL_LINE_SMOOTH));
                if (!smoothed)
                    glChecked(glEnable(GL_LINE_SMOOTH));
                GLfloat currentWidth = 0.f;
                glChecked(glGetFloatv(GL_LINE_WIDTH, &currentWidth));
                glChecked(glLineWidth(outlineThickness));
                glChecked(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]));
                filledShader->setUniform("fillColor", outline);
                glChecked(glDrawElements(GL_LINE_STRIP, outlineElementCount, GL_UNSIGNED_INT, (GLvoid*)0));
                // cleanup state
                glChecked(glLineWidth(currentWidth));
                if (!smoothed)
                    glChecked(glDisable(GL_LINE_SMOOTH));
            }
            glChecked(glDisableVertexAttribArray(posAttrib));
        }
    }

    Shape::Shape()
    {
        glChecked(glGenBuffers(buffers.size(), buffers.data()));
    }
    void Shape::setFilledElements(const std::vector<uint32_t>& elements)
    {
        GLint currentEbo = 0;
        glChecked(glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &currentEbo));
        // update our EBO.
        glChecked(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]));
        glChecked(glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(decltype(elements[0])), elements.data(), GL_STATIC_DRAW));
        glChecked(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, currentEbo));
        elementCount = elements.size();
    }

    void Shape::setFilledVertices(const std::vector<Vector2f>& vertices)
    {
        GLint currentVbo = 0;
        glChecked(glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &currentVbo));
        // update our VBO.
        glChecked(glBindBuffer(GL_ARRAY_BUFFER, buffers[0]));
        glChecked(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(decltype(vertices[0])), vertices.data(), GL_STATIC_DRAW));
        glChecked(glBindBuffer(GL_ARRAY_BUFFER, currentVbo));
    }

    void Shape::setOutlineElements(const std::vector<uint32_t>& elements)
    {
        GLint currentEbo = 0;
        glChecked(glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &currentEbo));
        // update our outline VBO.
        glChecked(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]));
        glChecked(glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(decltype(elements[0])), elements.data(), GL_STATIC_DRAW));
        glChecked(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, currentEbo));
        outlineElementCount = elements.size();
    }
#pragma endregion Shape
#pragma region Sprite
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

    void Sprite::draw(RenderTarget&, RenderStates) const
    {
        throw not_implemented();
    }
#pragma endregion Sprite
#pragma region Text
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
    void Text::draw(RenderTarget&, RenderStates) const
    {
        throw not_implemented();
    }
#pragma endregion Text
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
        if (sdlObject)
            return Vector2u(sdlObject->w, sdlObject->h);
        return { 0u, 0u };
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

#pragma
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
        glm::vec4 vec(point.x, point.y, 0.f, 1.f);
        auto result = matrix * vec;
        return Vector2f(result.x, result.y);
    }

#pragma region Transformable
    Transformable::~Transformable() = default;
    void Transformable::setScale(float factorX, float factorY)
    {
        scale.x = factorX;
        scale.y = factorY;
    }
    void Transformable::setOrigin(float x, float y)
    {
        setOrigin({ x, y });
    }
    void Transformable::setOrigin(const Vector2f& position)
    {
        origin = position;
    }
    void Transformable::setPosition(float x, float y)
    {
        setPosition({ x, y });
    }
    void Transformable::setPosition(const Vector2f& position)
    {
        this->position = position;
    }
    void Transformable::setRotation(float angle)
    {
        rotation = angle;
    }
#pragma endregion Transformable

#pragma region Vertex
    Vertex::Vertex()
        :position{0.f, 0.f}
        ,color{Color::Black}
        ,texCoords{0.f, 0.f}
    {
    }
#pragma endregion Vertex

#pragma region VertexArray
    VertexArray::VertexArray(PrimitiveType type, std::size_t vertexCount)
        :vertices(vertexCount),
        type{gl::primitive_cast(type)}
    {
        glChecked(glGenBuffers(2, buffers.data()));
    }
    VertexArray::~VertexArray()
    {
        glChecked(glDeleteBuffers(2, buffers.data()));
    }
    Vertex& VertexArray::operator [](std::size_t index)
    {
        // assume modify operation.
        dirty = true;
        return vertices[index];
    }
    const Vertex& VertexArray::operator [](std::size_t index) const
    {
        return vertices[index];
    }
    std::size_t VertexArray::getVertexCount() const
    {
        return vertices.size();
    }
    void VertexArray::draw(RenderTarget&target, RenderStates) const
    {
        glChecked(glBindBuffer(GL_ARRAY_BUFFER, buffers[0]));
        
        if (!elements.empty())
        {
            glChecked(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]));
        }

        if (dirty)
        {
            glChecked(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(decltype(vertices)::value_type), vertices.data(), GL_STATIC_DRAW));
            if (!elements.empty())
            {
                glChecked(glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(decltype(elements)::value_type), elements.data(), GL_STATIC_DRAW));
            }
            dirty = false;
        }
        glm::mat4 projection = glm::ortho(0.f, float(target.getSize().x), float(target.getSize().y), 0.f, -1.f, 1.f);
        glm::mat4 view = glm::identity<glm::mat4>();
        ScopedShader shader(DefaultShader);
        auto posAttrib = DefaultShader.attribute("position");
        auto colorAttrib = DefaultShader.attribute("color");
        DefaultShader.setUniform("projection", projection * view);
        glChecked(glEnableVertexAttribArray(posAttrib));
        glChecked(glEnableVertexAttribArray(colorAttrib));
        glChecked(glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0));
        glChecked(glVertexAttribPointer(colorAttrib, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (GLvoid*)sizeof(Vector2f)));
        if (!elements.empty())
        {
            glDrawElements(type, elements.size(), GL_UNSIGNED_INT, (GLvoid*)0);
        }
        else
        {
            glDrawArrays(type, 0, vertices.size());
        }
        glChecked(glDisableVertexAttribArray(colorAttrib));
        glChecked(glDisableVertexAttribArray(posAttrib));
    }

    void VertexArray::setElements(std::vector<uint32_t>&& elements)
    {
        this->elements = std::move(elements);
        dirty = true;
    }
#pragma endregion VertexArray
#pragma region View
    View::View()
        :View(FloatRect(0, 0, 1000, 1000)) // SFML impl has an arbitrary default size...
    {
    }
    View::View(const Vector2f& center, const Vector2f& size)
        :center(center), size(size),viewport(0.f, 0.f, 1.f, 1.f)
        
    {
    }
    View::View(const FloatRect& rectangle)
        :View(Vector2f(0, 0), Vector2f(0, 0))
    {
        center.x = rectangle.left + rectangle.width / 2.f;
        center.y = rectangle.top + rectangle.height / 2.f;
        size.x = rectangle.width;
        size.y = rectangle.height;
    }
    void View::setViewport(const FloatRect& theViewport)
    {
        viewport = theViewport;
    }
    const FloatRect& View::getViewport() const
    {
        return viewport;
    }
    const Vector2f& View::getSize() const
    {
        return size;
    }
    const Vector2f& View::getCenter() const
    {
        return center;
    }
#pragma endregion View
}