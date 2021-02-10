#ifndef SERIOUS_PROTON_SFML_OVER_SDL_SHADER_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_SHADER_HPP
#include <cstdint>
#include <string>
#include <SFML/Graphics/Glsl.hpp>
#include <SFML/System/InputStream.hpp>
#include <SFML/System/NonCopyable.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include "SDL_assert.h"

#include <array>
namespace sf
{
	class Shader : NonCopyable
	{
	public:
		enum Type
		{
			Vertex,
			Geometry,
			Fragment
		};
		static bool isAvailable();
		static void bind(const Shader* shader);
		bool loadFromFile(const std::string&, Type);
		bool loadFromStream(InputStream& vertexShaderStream, InputStream& fragmentShaderStream);

		template<typename T>
		void setUniform(const std::string& name, const T& x)
		{
			static_assert(false, "not implemented");
		}

		// Non-SFML
		int32_t attribute(const char* name) const;
		explicit operator bool() const { return program != 0; }
	private:
		uint32_t vertexShader = 0;
		uint32_t fragmentShader = 0;
		uint32_t program = 0;
		std::array<uint32_t, 8> textures;
		mutable uint8_t currentTextureUnit = 0;
	};
	class Texture;
#define SF_SHADER_DECLARE_UNIFORM(Type) \
	template<> void Shader::setUniform<Type>(const std::string&, const Type&); \
	extern template void Shader::setUniform(const std::string&, const Type&);
	// Base
	SF_SHADER_DECLARE_UNIFORM(bool);
	SF_SHADER_DECLARE_UNIFORM(float);
	// SFML
	SF_SHADER_DECLARE_UNIFORM(Color);
	SF_SHADER_DECLARE_UNIFORM(Vector2f);
	SF_SHADER_DECLARE_UNIFORM(Vector3f);
	SF_SHADER_DECLARE_UNIFORM(Texture);
	SF_SHADER_DECLARE_UNIFORM(Glsl::Vec4);
	// glm
	SF_SHADER_DECLARE_UNIFORM(glm::mat4);
	SF_SHADER_DECLARE_UNIFORM(glm::vec2);
	SF_SHADER_DECLARE_UNIFORM(glm::vec3);
	SF_SHADER_DECLARE_UNIFORM(glm::vec4);
	
#undef SF_SHADER_DECLARE_UNIFORM
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_SHADER_HPP