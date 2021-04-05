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

#include "GL/glad.h"

#include "SDL.h"
#define STB_RECT_PACK_IMPLEMENTATION
#define STBRP_STATIC
#define STBRP_ASSERT(x) SDL_assert(x)
#include "SFML/stb/stb_rect_pack.h"
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#define STBTT_assert(x) SDL_assert(x)
#include "SFML/stb/stb_truetype.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#define STBI_NO_STDIO
#define STBI_ASSERT(x) SDL_assert(x)
#include "SFML/stb/stb_image.h"
#include <glm/ext/matrix_float4x4.hpp> // mat4x4
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <stdexcept>
#include <mutex>
#include <chrono>
#include <unordered_map>

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
        auto beforeError = glGetError(); \
        SDL_assert(beforeError == GL_NO_ERROR); \
        call; \
        auto afterError = glGetError(); \
        SDL_assert(afterError == GL_NO_ERROR); \
    } while(false)
#else
#define glChecked(call) call
#endif
namespace sf
{
    namespace
    {
        namespace gl
        {
            constexpr GLenum primitive_cast(PrimitiveType type)
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
                    assert(false && "Quads are not supported!");
                }

                return GL_NONE;
            }

            constexpr GLenum blendfactor_cast(BlendMode::Factor factor)
            {
                switch (factor)
                {
                case BlendMode::Zero:
                    return GL_ZERO;
                case BlendMode::One:
                    return GL_ONE;
                case BlendMode::SrcColor:
                    return GL_SRC_COLOR;
                case BlendMode::SrcAlpha:
                    return GL_SRC_ALPHA;
                case BlendMode::OneMinusSrcAlpha:
                    return GL_ONE_MINUS_SRC_ALPHA;
                case BlendMode::OneMinusSrcColor:
                    return GL_ONE_MINUS_SRC_COLOR;
                case BlendMode::DstColor:
                    return GL_DST_COLOR;
                case BlendMode::OneMinusDstColor:
                    return GL_ONE_MINUS_DST_COLOR;
                case BlendMode::DstAlpha:
                    return GL_DST_ALPHA;
                case BlendMode::OneMinusDstAlpha:
                    return GL_ONE_MINUS_DST_ALPHA;
                }

                return GL_INVALID_ENUM;
            }

            constexpr GLenum blendequation_cast(BlendMode::Equation equation)
            {
                switch (equation)
                {
                case BlendMode::Add:
                    return GL_FUNC_ADD;
                case BlendMode::Subtract:
                    return GL_FUNC_SUBTRACT;
                case BlendMode::ReverseSubtract:
                    return GL_FUNC_REVERSE_SUBTRACT;
                }
                return GL_INVALID_ENUM;
            }
        }
        struct ScopedShader final
        {
            explicit ScopedShader(Shader& shader)
                :guarded{shader}
            {
                glChecked(glGetIntegerv(GL_CURRENT_PROGRAM, &previouslyBound));
                Shader::bind(&shader);
            }

            ~ScopedShader()
            {
                glChecked(glUseProgram(previouslyBound));
            }
            Shader& get() const { return guarded; }
        private:
            Shader& guarded;
            GLint previouslyBound = 0;
        };

        struct ScopedTexture final
        {
            explicit ScopedTexture(const Texture* texture)
                :guarded{ texture }
            {
                auto wantsToBind = texture ? texture->glObject : GL_NONE;
                glChecked(glGetIntegerv(GL_TEXTURE_BINDING_2D, &previouslyBound));
                if (previouslyBound != wantsToBind)
                {
                    Texture::bind(guarded);
                }
            }
            explicit ScopedTexture(const Texture& texture)
                :ScopedTexture(&texture)
            {
            }
            ~ScopedTexture()
            {
                auto currentlyBound = guarded ? guarded->glObject : GL_NONE;
                if (previouslyBound != currentlyBound)
                {
                    glChecked(glBindTexture(GL_TEXTURE_2D, previouslyBound));
                }
            }
            const Texture* get() const { return guarded; }
        private:
            const Texture* guarded = nullptr;
            GLint previouslyBound = 0;
        };

        struct ScopedRenderTarget final
        {
            explicit ScopedRenderTarget(const RenderTarget* texture)
                :guarded{ texture }
            {
                auto wantsToBind = texture ? texture->glObject : GL_NONE;
                glChecked(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previouslyBound));
                if (previouslyBound != wantsToBind)
                {
                    glChecked(glBindFramebuffer(GL_FRAMEBUFFER, wantsToBind));
                }
            }

            explicit ScopedRenderTarget(const RenderTarget& texture)
                :ScopedRenderTarget(&texture)
            {}

            ~ScopedRenderTarget()
            {
                auto currentlyBound = guarded ? guarded->glObject : GL_NONE;
                if (previouslyBound != currentlyBound)
                {
                    glChecked(glBindFramebuffer(GL_FRAMEBUFFER, previouslyBound));
                }
            }
            const RenderTarget* get() const { return guarded; }
        private:
            const RenderTarget* guarded = nullptr;
            GLint previouslyBound = 0;
        };

        struct ScopedBlending final
        {
        public:
            explicit ScopedBlending(BlendMode mode)
            {
                glChecked(wasBlending = glIsEnabled(GL_BLEND));
                if (!wasBlending)
                {
                    glChecked(glEnable(GL_BLEND));
                }
                else
                {
                    // Save state.
                    glChecked(glGetIntegerv(GL_BLEND_SRC_RGB, &srcRGB));
                    glChecked(glGetIntegerv(GL_BLEND_DST_RGB, &dstRGB));
                    glChecked(glGetIntegerv(GL_BLEND_SRC_ALPHA, &srcAlpha));
                    glChecked(glGetIntegerv(GL_BLEND_DST_ALPHA, &dstAlpha));
                    glChecked(glGetIntegerv(GL_BLEND_EQUATION_RGB, &modeRGB));
                    glChecked(glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &modeAlpha));
                }
                glChecked(glBlendFuncSeparate(
                    gl::blendfactor_cast(mode.color.src), gl::blendfactor_cast(mode.color.dst),
                    gl::blendfactor_cast(mode.alpha.src), gl::blendfactor_cast(mode.alpha.dst)
                ));
                glChecked(glBlendEquationSeparate(
                    gl::blendequation_cast(mode.color.blend),
                    gl::blendequation_cast(mode.alpha.blend)
                ));
            }

            ~ScopedBlending()
            {
                if (!wasBlending)
                {
                    glChecked(glDisable(GL_BLEND));
                }
                else
                {
                    glChecked(glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha));
                    glChecked(glBlendEquationSeparate(modeRGB, modeAlpha));
                }
            }
        private:
            BlendMode current;
            // Func
            GLint srcRGB = GL_NONE;
            GLint dstRGB = GL_NONE;
            GLint srcAlpha = GL_NONE;
            GLint dstAlpha = GL_NONE;
            // Eq
            GLint modeRGB = GL_NONE;
            GLint modeAlpha = GL_NONE;
            bool wasBlending = false;
        };

        

        constexpr const char vertexShader[] = R"glsl(#version 100
attribute vec2 position;
attribute vec4 color;
uniform mat4 projection;
varying vec4 vertexColor;
void main()
{
    gl_Position = projection * vec4(position, 0.0, 1.0);
    vertexColor = color;
})glsl";

        constexpr const char fragmentShader[] = R"glsl(#version 100
precision mediump float;
varying vec4 vertexColor;
void main()
{
    gl_FragColor = vertexColor;
})glsl";

        constexpr const char vertexArrayVertexShader[] = R"glsl(#version 100
attribute vec2 position;
attribute vec2 intex;
attribute vec4 color;
uniform mat4 projection;
uniform vec2 texSize;
varying vec4 vertexColor;
varying vec2 vertexTexCoords;
void main()
{
    gl_Position = projection * vec4(position, 0.0, 1.0);
    vertexColor = color;
    vertexTexCoords = intex / texSize;
})glsl";

        constexpr const char vertexArrayFragmentShader[] = R"glsl(#version 100
precision mediump float;
varying vec4 vertexColor;
varying vec2 vertexTexCoords;
uniform sampler2D tex;
uniform bool textured;
void main()
{
    if (!textured)
        gl_FragColor = vertexColor;
    else
        gl_FragColor = texture2D(tex, vertexTexCoords) * vertexColor;
})glsl";

        constexpr const char shapeVertexShader[] = R"glsl(#version 100
attribute vec2 position;
uniform mat4 projection;
void main()
{
    gl_Position = projection * vec4(position, 0.0, 1.0);
})glsl";

        constexpr const char shapeFragmentShader[] = R"glsl(#version 100
precision mediump float;
uniform vec4 fillColor;
void main()
{
    gl_FragColor = fillColor;
})glsl";

        constexpr const char shapeTexturedVertexShader[] = R"glsl(#version 100
uniform mat4 projection;
uniform vec4 texInfo;
uniform bool texFlip;
attribute vec2 position;
attribute vec2 intex;
varying vec2 fragtex;
void main()
{
    gl_Position = projection * vec4(position, 0.0, 1.0);
    
    fragtex = (texInfo.xy + intex) / texInfo.zw;
    if (texFlip)
        fragtex.y = fragtex.y * -1.0 + 1.0;
})glsl";
        constexpr const char shapeTexturedFragmentShader[] = R"glsl(#version 100
precision mediump float;
uniform sampler2D tex;
uniform vec4 fillColor;
uniform bool outline;
varying vec2 fragtex;
void main()
{
    if (!outline)
        gl_FragColor = texture2D(tex, fragtex) * fillColor;
    else
        gl_FragColor = fillColor;
})glsl";

        constexpr const char textFragmentShader[] = R"glsl(#version 100
precision mediump float;
uniform sampler2D tex;
uniform vec4 fillColor;
varying vec2 fragtex;
void main()
{
    gl_FragColor = vec4(1.0, 1.0, 1.0, texture2D(tex, fragtex).a) * fillColor;
})glsl";
    }

    namespace sdl
    {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        constexpr uint32_t rmask = 0xff000000;
        constexpr uint32_t gmask = 0x00ff0000;
        constexpr uint32_t bmask = 0x0000ff00;
        constexpr uint32_t amask = 0x000000ff;
#else
        constexpr uint32_t rmask = 0x000000ff;
        constexpr uint32_t gmask = 0x0000ff00;
        constexpr uint32_t bmask = 0x00ff0000;
        constexpr uint32_t amask = 0xff000000;
#endif

    }
    namespace stb
    {
        int read_rwops(void* user, char* data, int size)
        {
            auto ops = static_cast<SDL_RWops*>(user);
            return SDL_RWread(ops, data, 1, size);
        }
        void skip_rwops(void* user, int n)
        {
            auto ops = static_cast<SDL_RWops*>(user);
            SDL_RWseek(ops, n, RW_SEEK_CUR);
        }
        int eof_rwops(void* user)
        {
            auto ops = static_cast<SDL_RWops*>(user);
            auto pos = SDL_RWtell(ops);
            SDL_assert(pos != -1);
            return pos == SDL_RWsize(ops);
        }
        stbi_io_callbacks forRWops()
        {
            stbi_io_callbacks callbacks;
            callbacks.read = &read_rwops;
            callbacks.skip = &skip_rwops;
            callbacks.eof = &eof_rwops;
            return callbacks;
        }
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
        : color{ colorSourceFactor, colorDestinationFactor, colorBlendEquation }
        , alpha{ alphaSourceFactor, alphaDestinationFactor, alphaBlendEquation }
    {

    }

    BlendMode::BlendMode()
        :BlendMode(SrcAlpha, OneMinusSrcAlpha, Add, One, OneMinusSrcAlpha, Add)
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
        std::vector<VertexInfo> vertices(pointCount + 1); // points + center
        // First vertex is the center.
        vertices[0].position = { radius, radius };
        elements[0] = 0;
        for (size_t i = 1; i < pointCount + 1; ++i)
        {
            float angle = i * 2 * float(M_PI) / pointCount - float(M_PI) / 2;
            float x = std::cos(angle) * radius;
            float y = std::sin(angle) * radius;
            vertices[i].position = { radius + x, radius + y };
            elements[i] = static_cast<uint32_t>(i);
        }
        elements[pointCount + 1] = 1;
        setFilledElements(elements);
        setVertices(vertices);
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
    struct Font::Impl
    {
        struct Atlas final
        {
            ~Atlas() = default;
            std::vector<stbtt_packedchar> chars;
            Texture tex;
            Vector2u size = { 0, 0 };
            uint32_t rangeStart = 0;
        };
        ~Impl() = default;
        stbtt_fontinfo font;
        std::vector<uint8_t> fontData;
        std::unordered_map<uint32_t, Atlas> atlases;

        const Atlas& getAtlas(uint32_t characterSize)
        {
            auto candidate = atlases.emplace(characterSize, Atlas{});
            if (std::get<1>(candidate))
            {
                // Cache miss: Build it.
                // Pack font into texture.
                auto& atlas = std::get<1>(*std::get<0>(candidate));
                stbtt_pack_context context;
                std::vector<uint8_t> atlasData;
                atlas.chars.resize(256);
                auto scale = stbtt_ScaleForMappingEmToPixels(&font, STBTT_POINT_SIZE(int32_t(characterSize)));
                int lineGap = 0, ascent = 0, descent = 0;
                stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);
                auto lineHeight = scale * (ascent - descent + lineGap);
                auto charWidth = scale * STBTT_POINT_SIZE(int32_t(characterSize));
                atlas.size.x = ((characterSize + 4) * 128);
                atlas.size.y = ((characterSize + 4) * 2);
                for (; atlasData.empty();)
                {
                    atlasData.resize(size_t(atlas.size.x) * atlas.size.y);
                    if (stbtt_PackBegin(&context, atlasData.data(), atlas.size.x, atlas.size.y, atlas.size.x, 1, nullptr))
                    {
                        stbtt_PackSetOversampling(&context, 2, 2);
                        if (!stbtt_PackFontRange(&context, fontData.data(), 0, STBTT_POINT_SIZE(int32_t(characterSize)), 0, atlas.chars.size(), atlas.chars.data()))
                            atlasData.clear();
                        stbtt_PackEnd(&context);
                    }
                    else
                    {
                        atlasData.clear();
                    }
                }

                //atlas.size = atlas.size / 2;


                // Upload to GPU.
                SDL_assert(atlas.tex.glObject == GL_NONE);
                glChecked(glGenTextures(1, &atlas.tex.glObject));
                ScopedTexture guard{ atlas.tex };
                glChecked(glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, atlas.size.x, atlas.size.y, 0, GL_ALPHA, GL_UNSIGNED_BYTE, atlasData.data()));
                atlas.tex.setSmooth(true);
                atlas.tex.setRepeated(true);
                atlas.tex.forceUpdate();
            }

            return std::get<1>(*std::get<0>(candidate));
        }
    };
    Font::Font() = default;
    Font::~Font() = default;
    bool Font::loadFromStream(InputStream& stream)
    {
        if (!impl)
            impl = std::make_unique<Impl>();
        impl->fontData.clear();

        // Load font in buffer.
        {
            auto rw = rwops::fromStream(stream);
            SDL_assert(rw);
            auto fontSize = SDL_RWsize(rw);
            SDL_assert(fontSize > 0);
            impl->fontData.resize(fontSize);
            SDL_RWread(rw, impl->fontData.data(), 1, fontSize);
            SDL_RWclose(rw);
        }

       return stbtt_InitFont(&impl->font, impl->fontData.data(), stbtt_GetFontOffsetForIndex(impl->fontData.data(), 0)) != 0;
    }
    float Font::getLineSpacing(uint32_t characterSize) const
    {
        SDL_assert(impl);
        auto scale = stbtt_ScaleForMappingEmToPixels(&impl->font, characterSize);
        int lineGap = 0, ascent = 0, descent = 0;
        stbtt_GetFontVMetrics(&impl->font, &ascent, &descent, &lineGap);
        return scale * (ascent - descent + lineGap);
    }

    Glyph Font::getGlyph(Uint32 codePoint, uint32_t characterSize, bool bold, float outlineThickness) const
    {
        Glyph result;
        auto scale = stbtt_ScaleForMappingEmToPixels(&impl->font, characterSize);
        int advance = 0;
        stbtt_GetCodepointHMetrics(&impl->font, codePoint, &advance, nullptr);
        result.advance = advance * scale;
        return result;
    }
#pragma endregion Font

#pragma region Glsl
    namespace Glsl
    {
        Vec4::Vec4(const Color&c)
            :Vec4{ c.r / 255.f, c.g / 255.f, c.b / 255.f, c.a / 255.f }
        {
        }
        Vec4::Vec4(float x, float y, float z, float w)
            : vector{x, y, z, w}
        {}
    }
#pragma endregion Glsl

#pragma region Image
    namespace
    {
        void ensureImgLoaded()
        {
            
        }
    }
    void Image::create(unsigned int width, unsigned int height, const Color& color)
    {
        reset();
        pixels = static_cast<uint8_t*>(STBI_MALLOC(width * height * sizeof(uint32_t)));
        if (pixels)
        {
            union
            {
                uint32_t asRgba;
                struct
                {
                    uint8_t r;
                    uint8_t g;
                    uint8_t b;
                    uint8_t a;
                } comp;
            } fillColor;
            fillColor.comp.r = color.r;
            fillColor.comp.g = color.g;
            fillColor.comp.b = color.b;
            fillColor.comp.a = color.a;
            const auto pixelCount = size_t(width) * height;
            SDL_memset4(pixels, fillColor.asRgba, size_t(width)* height);
        }
    }
    Vector2u Image::getSize() const
    {
        return size;
    }
    bool Image::loadFromStream(InputStream& stream)
    {
        reset();
        auto ops = rwops::fromStream(stream);
        if (!ops)
            return false;
        auto stbcallbacks = stb::forRWops();
        int width = 0, height = 0, components = 0;

        pixels = stbi_load_from_callbacks(&stbcallbacks, ops, &width, &height, nullptr, STBI_rgb_alpha);
        if (pixels)
        {
            size.x = width;
            size.y = height;
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Failed to load image: %s", stbi_failure_reason());
        }
        SDL_RWclose(ops);
        
        return pixels != nullptr;
    }
    Color Image::getPixel(uint32_t x, uint32_t y) const
    {
        SDL_assert(pixels != nullptr);
        Color result;
        auto pixel = pixels + x + size_t(y) * getSize().x;
        return Color(pixel[0], pixel[1], pixel[2], pixel[3]);
    }
    Image::~Image()
    {
        stbi_image_free(pixels);
    }
    void Image::reset()
    {
        if (pixels)
        {
            stbi_image_free(pixels);
            pixels = nullptr;
            size = { 0u, 0u };
        }
    }
#pragma endregion Image

#pragma region RectangleShape
    RectangleShape::RectangleShape(const Vector2f& size)
    {
        setSize(size);
        setFilledElements(std::initializer_list<uint8_t>{ 0, 1, 2, 2, 3, 0 });
        setOutlineElements(std::initializer_list<uint8_t>{ 0, 1, 2, 3, 0 });
    }

    void RectangleShape::setSize(const Vector2f& size)
    {
        setSizeAndTexCoords(size, { 0.f, 0.f, 1.f, 1.f });
    }
    const Vector2f& RectangleShape::getSize() const
    {
        return size;
    }
    void RectangleShape::setSizeAndTexCoords(const Vector2f& size, const FloatRect& tex)
    {
        this->size = size;
        setVertices({
            {{0, 0}, {tex.left, tex.top}},
            {{size.x, 0}, {tex.width, tex.top}},
            {size, {tex.width, tex.height}},
            {{0, size.y}, {tex.left, tex.height}}
        });
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
        :texture(theTexture)
    {
    }

#pragma region RenderTarget
    RenderTarget::~RenderTarget()
    {
        glChecked(glDeleteFramebuffers(1, &glObject));
    }
    sf::Vector2u RenderTarget::getSize() const
    {
        throw not_implemented();
    }
    IntRect RenderTarget::getViewport(const View& view) const
    {
        auto width = static_cast<float>(getSize().x);
        auto height = static_cast<float>(getSize().y);
        const FloatRect& viewport = view.getViewport();

        return IntRect{ static_cast<int32_t>(std::round(width * viewport.left)),
            static_cast<int32_t>(std::round(height * viewport.top)),
            static_cast<int32_t>(std::round(width * viewport.width)),
            static_cast<int32_t>(std::round(height * viewport.height)) };
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
        ScopedRenderTarget guard{ this };
        glClearColor(color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void RenderTarget::draw(const Drawable& drawable, const RenderStates& states)
    {
        ScopedRenderTarget guard{ this };
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
        ScopedBlending blending{ states.blend };
        ScopedTexture stateTex{ states.texture };
        drawable.draw(*this, states);
    }
    Vector2f RenderTarget::mapPixelToCoords(const Vector2i& point) const
    {
        return Vector2f(float(point.x), float(point.y));
    }
    Vector2i RenderTarget::mapCoordsToPixel(const Vector2f& point) const
    {
#if 0
        // First, transform the point by the view matrix
        auto normalized = point;
        // Then convert to viewport coordinates
        Vector2i pixel;
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
        pixel.x = static_cast<int>((normalized.x + 1.f) / 2.f * viewportGl.width + viewportGl.left);
        pixel.y = static_cast<int>((-normalized.y + 1.f) / 2.f * viewportGl.height + top);

        return pixel;
#else
        return Vector2i(static_cast<int32_t>(point.x), static_cast<int32_t>(point.y));
#endif
    }
    void RenderTarget::popGLStates()
    {
        Texture::bind(nullptr);
        Shader::bind(nullptr);
    }
    void RenderTarget::pushGLStates()
    {
        glChecked(glDisable(GL_DEPTH_TEST));
    }

    void RenderTarget::resetGLStates()
    {
        Texture::bind(nullptr);
        Shader::bind(nullptr);

        glChecked(glDisable(GL_DEPTH_TEST));
        glChecked(glDisable(GL_CULL_FACE));
    }
#pragma endregion RenderTarget
#pragma region RenderTexture
    RenderTexture::RenderTexture()
    {
        texture.setFlipped(true);
    }
    RenderTexture::~RenderTexture()
    {
        glChecked(glDeleteRenderbuffers(1, &rbo));
    }
    bool RenderTexture::create(unsigned int width, unsigned int height, bool depthBuffer)
    {
        if (glObject == GL_NONE)
        {
            glChecked(glGenFramebuffers(1, &glObject));
            glChecked(glGenTextures(1, &texture.glObject));
        }

        ScopedRenderTarget guard{ this };
        ScopedTexture guardTex{ texture };
        glChecked(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
        texture.size = { width, height };
        texture.forceUpdate();
        glChecked(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.glObject, 0));
        if (depthBuffer)
        {
            if (rbo == GL_NONE)
            {
                glChecked(glGenRenderbuffers(1, &rbo));
                glChecked(glBindRenderbuffer(GL_RENDERBUFFER, rbo));
                glChecked(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height));
                glChecked(glBindRenderbuffer(GL_RENDERBUFFER, GL_NONE));
            }

            glChecked(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo));
        }
        else if (rbo != GL_NONE)
        {
            glChecked(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, GL_NONE));
        }

        auto status = GL_NONE;
        glChecked(status = glCheckFramebufferStatus(GL_FRAMEBUFFER));
        return status == GL_FRAMEBUFFER_COMPLETE;
    }
    void RenderTexture::setRepeated(bool repeated)
    {
        texture.setRepeated(repeated);
    }
    void RenderTexture::setSmooth(bool smooth)
    {
        texture.setSmooth(smooth);
    }
    void RenderTexture::display()
    {
        // noop?
    }
    const Texture& RenderTexture::getTexture() const
    {
        return texture;
    }
    sf::Vector2u RenderTexture::getSize() const
    {
        return texture.getSize();
    }
#pragma endregion RenderTexture
#pragma region RenderWindow
    struct RenderWindow::Impl final
    {
        SDL_Window* window = nullptr;
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
        SDL_assert(impl && impl->window);
        int w = 0, h = 0;
        SDL_GL_GetDrawableSize(impl->window, &w, &h);
        return sf::Vector2u(w, h);
    }
    void RenderWindow::display()
    {
        if (impl)
        {
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
        impl = std::make_unique<Impl>();
    }

    void RenderWindow::create(VideoMode mode, const std::string& title, Uint32 style, const ContextSettings& settings)
    {
        if (impl->window)
        {
            SDL_SetWindowSize(impl->window, mode.width, mode.height);
            SDL_SetWindowResizable(impl->window, (style & Style::Resize) ? SDL_TRUE : SDL_FALSE);
            SDL_SetWindowFullscreen(impl->window, (style & Style::Fullscreen) ? SDL_TRUE : SDL_FALSE);
            SDL_SetWindowBordered(impl->window, (style & Style::Titlebar) ? SDL_TRUE : SDL_FALSE);
            setTitle(title);
            return;
        }
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE | SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, settings.majorVersion);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, settings.minorVersion);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, settings.depthBits);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, settings.stencilBits);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, settings.antialiasingLevel > 0 ? 1 : 0);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, settings.antialiasingLevel);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
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
        impl->window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, mode.width, mode.height, flags);
        if (!impl->window)
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
        gladLoadGLES2Loader(SDL_GL_GetProcAddress);
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
    RenderWindow::Impl* RenderWindow::getImpl() const
    {
        return impl.get();
    }
    Vector2i Mouse::getPosition(const RenderWindow&)
    {
        Vector2i pos;
        SDL_GetMouseState(&pos.x, &pos.y);
        return pos;
    }

    void Mouse::setPosition(const sf::Vector2i&pos, RenderWindow&window)
    {
        auto pimpl = window.getImpl();
        auto sdlWindow = pimpl ? pimpl->window : nullptr;
        SDL_WarpMouseInWindow(sdlWindow, pos.x, pos.y);
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
            for (auto i = 0; i < shader->currentTextureUnit; ++i)
            {
                glChecked(glActiveTexture(GL_TEXTURE0 + i));
                glChecked(glBindTexture(GL_TEXTURE_2D, shader->textures[i]));
            }
            if (shader->currentTextureUnit > 1)
            {
                glChecked(glActiveTexture(GL_TEXTURE0));
            }
            shader->currentTextureUnit = 0;
        }


        glChecked(glUseProgram(program));
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
                std::vector<char> code(length + 1);
                if (SDL_RWread(sdlStream.get(), code.data(), code.size() - 1, 1) == 0)
                {
                    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Empty or error shader stream: %s", SDL_GetError());
                    return false;
                }
                code[length] = '\0';

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
    void Shader::setUniform(const std::string& name, const bool& value)
    {
        GLint location = 0;
        glChecked(location = glGetUniformLocation(program, name.c_str()));
        glChecked(glUniform1i(location, value));
    }
    template<>
    void Shader::setUniform(const std::string& name, const float& value)
    {
        GLint location = 0;
        glChecked(location = glGetUniformLocation(program, name.c_str()));
        glChecked(glUniform1f(location, value));
    }
    // SFML
    template<>
    void Shader::setUniform(const std::string& name, const Color& color)
    {
        setUniform(name, Glsl::Vec4(color));
    }
    template<>
    void Shader::setUniform(const std::string& name, const Vector2f& value)
    {
        GLint location = -1;
        glChecked(location = glGetUniformLocation(program, name.c_str()));
        glChecked(glUniform2f(location, value.x, value.y));
    }
    template<>
    void Shader::setUniform(const std::string& name, const Vector3f& value)
    {
        GLint location = -1;
        glChecked(location = glGetUniformLocation(program, name.c_str()));
        glChecked(glUniform3f(location, value.x, value.y, value.z));
    }
    template<>
    void Shader::setUniform(const std::string& name, const Texture& value)
    {
        glChecked(glActiveTexture(GL_TEXTURE0 + currentTextureUnit));
        Texture::bind(&value);
        textures[currentTextureUnit++] = value.glObject;
        glChecked(glActiveTexture(GL_TEXTURE0));
    }
    template<>
    void Shader::setUniform(const std::string& name, const Glsl::Vec4& value)
    {
        setUniform(name, value.vector);
    }
    // glm
    template<>
    void Shader::setUniform(const std::string& name, const glm::mat4&matrix)
    {
        GLint location = 0;
        glChecked(location = glGetUniformLocation(program, name.c_str()));
        glChecked(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix)));
    }
    template<>
    void Shader::setUniform(const std::string& name, const glm::vec2& vector)
    {
        GLint location = 0;
        glChecked(location = glGetUniformLocation(program, name.c_str()));
        glChecked(glUniform2fv(location, 1, glm::value_ptr(vector)));
    }
    template<>
    void Shader::setUniform(const std::string& name, const glm::vec3& vector)
    {
        GLint location = 0;
        glChecked(location = glGetUniformLocation(program, name.c_str()));
        glChecked(glUniform3fv(location, 1, glm::value_ptr(vector)));
    }
    template<>
    void Shader::setUniform(const std::string& name, const glm::vec4& vector)
    {
        GLint location = 0;
        glChecked(location = glGetUniformLocation(program, name.c_str()));
        glChecked(glUniform4fv(location, 1, glm::value_ptr(vector)));
    }
#pragma endregion Shader
#pragma region Shape
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
        static Shader filledShader;
        if (!filledShader)
        {
            sf::MemoryInputStream vertexShaderStream, fragmentShaderStream;
            vertexShaderStream.open(shapeVertexShader, strlen(shapeVertexShader) + 1);
            fragmentShaderStream.open(shapeFragmentShader, strlen(shapeFragmentShader) + 1);
            filledShader.loadFromStream(vertexShaderStream, fragmentShaderStream);
        }

        static Shader texturedShader;
        if (!texturedShader)
        {
            sf::MemoryInputStream vertexShaderStream, fragmentShaderStream;
            vertexShaderStream.open(shapeTexturedVertexShader, strlen(shapeTexturedVertexShader) + 1);
            fragmentShaderStream.open(shapeTexturedFragmentShader, strlen(shapeTexturedFragmentShader) + 1);
            texturedShader.loadFromStream(vertexShaderStream, fragmentShaderStream);
        }

        // Skip if it's an empty shape,
        // or it's fully transparent.
        const auto isTransparent = fill.a == 0 && outline.a == 0;
        if (elementCount > 0 && !isTransparent)
        {
            // Setup shader.
            ScopedShader guard(texture ? texturedShader : filledShader);
            ScopedTexture textureGuard{ texture };
            // Constants (uniforms)
            const auto& view = target.getView();
            auto topLeft = view.getCenter() - view.getSize() / 2.f;
            auto bottomRight = view.getCenter() + view.getSize() / 2.f;
            auto projection = glm::ortho(
                topLeft.x, bottomRight.x, bottomRight.y, topLeft.y
            );
           // glm::mat4 projection = glm::ortho(0.f, float(target.getView().getSize().x), float(target.getView().getSize().y), 0.f, -1.f, 1.f);
            glm::mat4 model = getTransform();
            guard.get().setUniform("projection", projection * model);
            guard.get().setUniform("fillColor", fill);
            guard.get().setUniform("outline", false);
            glm::vec4 texInfo{ 0.f, 0.f, 1.f, 1.f };
            if (texture)
            {
                texInfo.x = getTextureRect().left;
                texInfo.y = getTextureRect().top;
                texInfo.z = float(texture->getSize().x) / getTextureRect().width;
                texInfo.w = float(texture->getSize().y) / getTextureRect().height;
            }
            guard.get().setUniform("texFlip", texture ? texture->isFlipped() : false);
            guard.get().setUniform("texInfo", texInfo);

            // Buffers
            glChecked(glBindBuffer(GL_ARRAY_BUFFER, buffers[0]));
            glChecked(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]));

            // Vertex attributes
            auto posAttrib = guard.get().attribute("position");
            glChecked(glEnableVertexAttribArray(posAttrib));
            glChecked(glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(VertexInfo), (GLvoid*)0));
            auto texAttrib = guard.get().attribute("intex");
            if (texAttrib >= 0)
            {
                glChecked(glEnableVertexAttribArray(texAttrib));
                glChecked(glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(VertexInfo), (GLvoid*)sizeof(Vector2f)));

            }
            // Don't draw if transparent fill.
            if (fill.a > 0)
            {
                // Filled shape.
                glChecked(glDrawElements(gl::primitive_cast(getType()), elementCount, filledElementType, (GLvoid*)0));
            }

            // outline
            if (outlineThickness > 0.f && outline.a > 0)
            {
                guard.get().setUniform("outline", true);
                // setup state.
                auto smoothed = false;
                GLfloat currentWidth = 0.f;
                glChecked(glGetFloatv(GL_LINE_WIDTH, &currentWidth));
                glChecked(glLineWidth(outlineThickness));
                glChecked(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]));
                guard.get().setUniform("fillColor", outline);
                glChecked(glDrawElements(GL_LINE_STRIP, outlineElementCount, outlineElementType, (GLvoid*)0));
                // cleanup state
                glChecked(glLineWidth(currentWidth));
            }
            if (texAttrib >= 0)
            {
                glChecked(glDisableVertexAttribArray(texAttrib));
            }
            glChecked(glDisableVertexAttribArray(posAttrib));
        }
    }

    Shape::Shape()
    {
        glChecked(glGenBuffers(buffers.size(), buffers.data()));
    }
    void Shape::setVertices(std::initializer_list<VertexInfo> vertices)
    {
        setVertices(std::begin(vertices), vertices.size());
    }
    void Shape::setVertices(const std::vector<VertexInfo>& vertices)
    {
        setVertices(vertices.data(), vertices.size());
    }
    void Shape::setFilledElements(const void* elements, size_t typeSize, size_t count)
    {
        GLint currentEbo = 0;
        glChecked(glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &currentEbo));
        // update our EBO.
        glChecked(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[buffer_cast(Buffer::ElementsFilled)]));
        glChecked(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * typeSize, elements, GL_STATIC_DRAW));
        glChecked(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, currentEbo));
        elementCount = count;
        switch (typeSize)
        {
        case 1:
            filledElementType = GL_UNSIGNED_BYTE;
            break;
        case 2:
            filledElementType = GL_UNSIGNED_SHORT;
            break;
        case 4:
            filledElementType = GL_UNSIGNED_INT;
            break;
        default:
            SDL_assert(false);
        }
    }
    void Shape::setVertices(const VertexInfo* vertices, size_t count)
    {
        GLint currentVbo = 0;
        glChecked(glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &currentVbo));
        // update our VBO.
        glChecked(glBindBuffer(GL_ARRAY_BUFFER, buffers[buffer_cast(Buffer::Vertex)]));
        glChecked(glBufferData(GL_ARRAY_BUFFER, count * sizeof(decltype(*vertices)), vertices, GL_STATIC_DRAW));
        glChecked(glBindBuffer(GL_ARRAY_BUFFER, currentVbo));
    }
    void Shape::setOutlineElements(const void* elements, size_t typeSize, size_t count)
    {
        GLint currentEbo = 0;
        glChecked(glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &currentEbo));
        // update our outline VBO.
        glChecked(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[buffer_cast(Buffer::ElementsOutline)]));
        glChecked(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * typeSize, elements, GL_STATIC_DRAW));
        glChecked(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, currentEbo));
        outlineElementCount = count;
        switch (typeSize)
        {
        case 1:
            outlineElementType = GL_UNSIGNED_BYTE;
            break;
        case 2:
            outlineElementType = GL_UNSIGNED_SHORT;
            break;
        case 4:
            outlineElementType = GL_UNSIGNED_INT;
            break;
        default:
            SDL_assert(false);
        }
    }
#pragma endregion Shape
#pragma region Sprite
    Sprite::Sprite() = default;
    Sprite::Sprite(const Texture& texture)
    {
        setTexture(texture);
    }
    Sprite::Sprite(const Texture& texture, const IntRect& rectangle)
        :Sprite(texture)
    {
        setTextureRect(rectangle);
    }

    void Sprite::setTexture(const Texture& texture, bool resetRect)
    {
        impl.setTexture(&texture, resetRect);
        impl.setSize(Vector2f{ float(getTextureRect().width), float(getTextureRect().height) });
    }
    void Sprite::setTextureRect(const IntRect& rectangle)
    {
        impl.setTextureRect(rectangle);
        impl.setSize(Vector2f{ float(getTextureRect().width), float(getTextureRect().height) });
    }
    const IntRect& Sprite::getTextureRect() const
    {
        return impl.getTextureRect();
    }
    void Sprite::setColor(const Color& color)
    {
        impl.setFillColor(color);
    }

    void Sprite::draw(RenderTarget& target, RenderStates states) const
    {
        impl.setTransform(*this);
        impl.draw(target, states);
    }
#pragma endregion Sprite
#pragma region Text
    Text::Text(const String& string, const Font& font, unsigned int characterSize)
        :textLength{string.getSize()}
    {
        glChecked(glGenBuffers(buffers.size(), buffers.data()));
        std::vector<VertexInfo> vertices;
        std::vector<uint32_t> elements;

        vertices.reserve(textLength * 4); // 4 vertices per glyph (1 quad)
        elements.reserve(textLength * 6); // 6 elements each (2 triangles)
        const auto& atlas = font.impl->getAtlas(characterSize);
        // Cache values
        const auto lineSpacing = font.getLineSpacing(characterSize);
        const auto whitespaceLength = font.getGlyph(L' ', characterSize, false).advance;
        // Fill buffers with text.
        stbtt_aligned_quad quad;
        float x = 0.f, y = lineSpacing;

        for (auto i = 0; i < textLength; ++i)
        {
            auto character = string[i];
            if (character == L'\r')
                continue;
            if (character == L'\n')
            {
                // reset x, advance y.
                x = 0.f;
                y += lineSpacing;
                continue;
            }
            if (character == L'\t')
            {
                // advance by 4 spaces (in line with SFML implementation)
                x += whitespaceLength * 4;
                continue;
            }
            if (character == L' ')
            {
                x += whitespaceLength;
                continue;
            }
            stbtt_GetPackedQuad(atlas.chars.data(), atlas.size.x, atlas.size.y, character, &x, &y, &quad, 1);
            
            const auto quadIndex = vertices.size() / 4;
            vertices.emplace_back(VertexInfo{ Vector2f{ quad.x0, quad.y0 }, Vector2f{quad.s0, quad.t0} });
            vertices.emplace_back(VertexInfo{ Vector2f{ quad.x1, quad.y0 }, Vector2f{quad.s1, quad.t0} });
            vertices.emplace_back(VertexInfo{ Vector2f{ quad.x1, quad.y1 }, Vector2f{quad.s1, quad.t1} });
            vertices.emplace_back(VertexInfo{ Vector2f{ quad.x0, quad.y1 }, Vector2f{quad.s0, quad.t1} });

            elements.emplace_back(4 * quadIndex + 0);
            elements.emplace_back(4 * quadIndex + 1);
            elements.emplace_back(4 * quadIndex + 2);
            elements.emplace_back(4 * quadIndex + 2);
            elements.emplace_back(4 * quadIndex + 3);
            elements.emplace_back(4 * quadIndex + 0);

            extents.x = std::max(extents.x, std::max(quad.x0, quad.x1));
            extents.y = std::max(extents.y, std::max(quad.y0, quad.y1));
        }

        // Upload
        {
            GLint currentEbo = 0;
            glChecked(glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &currentEbo));
            // update our EBO.
            glChecked(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]));
            glChecked(glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(decltype(elements[0])), elements.data(), GL_STATIC_DRAW));
            glChecked(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, currentEbo));
        }
        {
            GLint currentVbo = 0;
            glChecked(glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &currentVbo));
            // update our VBO.
            glChecked(glBindBuffer(GL_ARRAY_BUFFER, buffers[0]));
            glChecked(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(decltype(vertices[0])), vertices.data(), GL_STATIC_DRAW));
            glChecked(glBindBuffer(GL_ARRAY_BUFFER, currentVbo));
        }

        texture = &atlas.tex;
    }

    Text::~Text()
    {
        glChecked(glDeleteBuffers(buffers.size(), buffers.data()));
    }
    void Text::setColor(const Color& color)
    {
        this->fill = color;
    }
    FloatRect Text::getLocalBounds() const
    {
        return { 0.f, 0.f, extents.x, extents.y };
    }
    void Text::draw(RenderTarget&target, RenderStates states) const
    {
        constexpr bool useTextSpecific = true;
        if constexpr (useTextSpecific)
        {
            static Shader texturedShader;
            if (!texturedShader)
            {
                sf::MemoryInputStream vertexShaderStream, fragmentShaderStream;
                vertexShaderStream.open(shapeTexturedVertexShader, strlen(shapeTexturedVertexShader) + 1);
                fragmentShaderStream.open(textFragmentShader, strlen(textFragmentShader) + 1);
                texturedShader.loadFromStream(vertexShaderStream, fragmentShaderStream);
            }


            // Setup shader.
            ScopedShader guard(texturedShader);
            ScopedTexture textureGuard(texture);
            // Constants (uniforms)
            const auto& view = target.getView();
            auto projection = glm::ortho(
                view.getCenter().x - view.getSize().x / 2.f,
                view.getCenter().x + view.getSize().x / 2.f,
                view.getCenter().y + view.getSize().y / 2.f,
                view.getCenter().y - view.getSize().y / 2.f);
            //glm::mat4 projection = glm::ortho(0.f, float(target.getView().getSize().x), float(target.getView().getSize().y), 0.f, -1.f, 1.f);
            glm::mat4 model = getTransform();
            guard.get().setUniform("projection", projection * model);
            guard.get().setUniform("texInfo", glm::vec4{ 0.f, 0.f, 1.f, 1.f });
            guard.get().setUniform("texFlip", false);
            guard.get().setUniform("fillColor", fill);
            // Buffers
            glChecked(glBindBuffer(GL_ARRAY_BUFFER, buffers[0]));
            glChecked(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]));

            // Vertex attributes
            auto posAttrib = guard.get().attribute("position");
            constexpr auto vertexTypeSize = sizeof(VertexInfo);
            glChecked(glEnableVertexAttribArray(posAttrib));
            glChecked(glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, vertexTypeSize, (GLvoid*)0));
            auto texAttrib = guard.get().attribute("intex");
            glChecked(glEnableVertexAttribArray(texAttrib));
            glChecked(glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, vertexTypeSize, (GLvoid*)sizeof(Vector2f)));
            glChecked(glDrawElements(GL_TRIANGLES, textLength * 6, GL_UNSIGNED_INT, (GLvoid*)0));
            glChecked(glDisableVertexAttribArray(texAttrib));
            glChecked(glDisableVertexAttribArray(posAttrib));
        }
    }
#pragma endregion Text
    // Texture
    void Texture::bind(const Texture* texture, CoordinateType coordinateType)
    {
        SDL_assert(coordinateType == CoordinateType::Normalized);
        auto textureID = texture ? texture->glObject : GL_NONE;
        glChecked(glBindTexture(GL_TEXTURE_2D, textureID));
#if 0
        if (textureID)
        {
            // SFML compat.
            if ((coordinateType == Pixels) || texture->flipped)
            {
                GLfloat matrix[16] = { 1.f, 0.f, 0.f, 0.f,
                                      0.f, 1.f, 0.f, 0.f,
                                      0.f, 0.f, 1.f, 0.f,
                                      0.f, 0.f, 0.f, 1.f };

                // If non-normalized coordinates (= pixels) are requested, we need to
                // setup scale factors that convert the range [0 .. size] to [0 .. 1]
                if (coordinateType == Pixels)
                {
                    matrix[0] = 1.f / texture->getSize().x;
                    matrix[5] = 1.f / texture->getSize().y;
                }

                // If pixels are flipped we must invert the Y axis
                if (texture->flipped)
                {
                    matrix[5] = -matrix[5];
                    matrix[13] = static_cast<float>(texture->getSize().y) / texture->getSize().y;
                }

                // Load the matrix
                glChecked(glMatrixMode(GL_TEXTURE));
                glChecked(glLoadMatrixf(matrix));
                // Go back to model-view mode (sf::RenderTarget relies on it)
                glChecked(glMatrixMode(GL_MODELVIEW));
            }
        }
#endif
    }
    Texture::Texture()
        :repeated(false), smooth(true), flipped(false)
    {
    }

    Texture::~Texture()
    {
        glChecked(glDeleteTextures(1, &glObject));
    }
    Vector2u Texture::getSize() const
    {
        return size;
    }
    bool Texture::loadFromImage(const Image& image, const IntRect& area)
    {
        if (!*this)
        {
            glChecked(glGenTextures(1, &glObject));
        }
        ScopedTexture guard{ *this };
        // In line with SFML implementation.
        auto imageSize = image.getSize();
        size_t offset = 0;
        if (area.width == 0 || (area.height == 0) ||
            ((area.left <= 0) && (area.top <= 0) && (area.width >= imageSize.x) && (area.height >= imageSize.y)))
        {
            size = image.getSize();
            glChecked(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data() + offset));
        }
        else
        {
            // Adjust the rectangle to the size of the image (SFML)
            IntRect rectangle = area;
            if (rectangle.left < 0) rectangle.left = 0;
            if (rectangle.top < 0) rectangle.top = 0;
            if (rectangle.left + rectangle.width > imageSize.x)  rectangle.width = imageSize.x - rectangle.left;
            if (rectangle.top + rectangle.height > imageSize.y) rectangle.height = imageSize.y - rectangle.top;

            size = { static_cast<uint32_t>(rectangle.width), static_cast<uint32_t>(rectangle.height) };
            // Create the (empty) texture
            glChecked(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
            offset = 4 * (rectangle.left + (size_t(imageSize.x) * rectangle.top));
            for (int i = 0; i < rectangle.height; ++i)
            {
                glChecked(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, i, rectangle.width, 1, GL_RGBA, GL_UNSIGNED_BYTE, image.data() + offset));
                offset += 4 * size_t(imageSize.x);
            }
        }
        forceUpdate();
        return true;
    }

    void Texture::setRepeated(bool inRepeated)
    {
        if (repeated != inRepeated)
        {
            repeated = inRepeated;
            if (glObject)
            {
                ScopedTexture guard{ *this };
                updateRepeat();
            }
        }
    }
    void Texture::setSmooth(bool inSmooth)
    {
        if (smooth != inSmooth)
        {
            smooth = inSmooth;
            if (glObject)
            {
                ScopedTexture guard{ *this };
                updateRepeat();
            }
        }
    }
    void Texture::setFlipped(bool flipped)
    {
        this->flipped = flipped;
    }
    bool Texture::isFlipped() const
    {
        return flipped;
    }

    void Texture::forceUpdate()
    {
        updateRepeat();
        updateSmooth();
    }

    void Texture::updateRepeat()
    {
        glChecked(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeated ? GL_REPEAT : GL_CLAMP_TO_EDGE));
        glChecked(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeated ? GL_REPEAT : GL_CLAMP_TO_EDGE));
    }
    void Texture::updateSmooth()
    {
        glChecked(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, smooth ? GL_LINEAR : GL_NEAREST));
        glChecked(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, smooth ? GL_LINEAR : GL_NEAREST));
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
        // https://stackoverflow.com/a/31721700
        matrix[0] = glm::vec4(a00, a01, 0,  a02);
        matrix[1] = glm::vec4(a10, a11, 0, a12);
        matrix[2] = glm::vec4(0, 0, 1, 0);
        matrix[3] = glm::vec4(a20, a21, 0, a22);
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
    glm::mat4 Transformable::getTransform() const
    {
        // Offset center
        auto xform = glm::translate(glm::identity<glm::mat4>(), glm::vec3(origin.x, origin.y, 0.f));
        // Rotate
        xform = glm::rotate(xform, glm::radians(rotation), glm::vec3(0.f, 0.f, 1.f));
        // Scale
        xform = glm::scale(xform, glm::vec3(scale.x, scale.y, 1.f));
        // Undo origin change
        xform = glm::translate(xform, -glm::vec3(origin.x, origin.y, 0.f));
        // Apply position to transformed object
        return glm::translate(glm::identity<glm::mat4>(), glm::vec3(position.x - origin.x, position.y - origin.y, 0.f)) * xform;
    }
    void Transformable::setTransform(const Transformable& other)
    {
        scale = other.scale;
        origin = other.origin;
        position = other.position;
        rotation = other.rotation;
    }
#pragma endregion Transformable

#pragma region Vertex
    Vertex::Vertex()
        :position{0.f, 0.f}
        ,color{Color::White}
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
    void VertexArray::draw(RenderTarget&target, RenderStates states) const
    {
        static Shader shader;
        if (!shader)
        {
            sf::MemoryInputStream vertexShaderStream, fragmentShaderStream;
            vertexShaderStream.open(vertexArrayVertexShader, strlen(vertexArrayVertexShader) + 1);
            fragmentShaderStream.open(vertexArrayFragmentShader, strlen(vertexArrayFragmentShader) + 1);
            shader.loadFromStream(vertexShaderStream, fragmentShaderStream);
        }

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
        const auto& view = target.getView();
        auto projection = glm::ortho(
            view.getCenter().x - view.getSize().x / 2.f,
            view.getCenter().x + view.getSize().x / 2.f,
            view.getCenter().y + view.getSize().y / 2.f,
            view.getCenter().y - view.getSize().y / 2.f);
        //glm::mat4 projection = glm::ortho(0.f, float(target.getView().getSize().x), float(target.getView().getSize().y), 0.f, -1.f, 1.f);
        ScopedShader guard(shader);
        auto posAttrib = guard.get().attribute("position");
        auto colorAttrib = guard.get().attribute("color");
        auto texAttrib = guard.get().attribute("intex");
        guard.get().setUniform("projection", projection);
        guard.get().setUniform("textured", states.texture != nullptr);
        Vector2f texSize{ states.texture ? Vector2f(states.texture->getSize()) : Vector2f{1.f, 1.f} };
        guard.get().setUniform("texSize", texSize);
        guard.get().setUniform("texFlip", states.texture ? states.texture->isFlipped() : false);
        glChecked(glEnableVertexAttribArray(posAttrib));
        glChecked(glEnableVertexAttribArray(colorAttrib));
        glChecked(glEnableVertexAttribArray(texAttrib));
        glChecked(glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0));
        glChecked(glVertexAttribPointer(colorAttrib, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (GLvoid*)sizeof(Vector2f)));
        glChecked(glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(Vector2f) + sizeof(Color))));
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
        glChecked(glDisableVertexAttribArray(texAttrib));
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