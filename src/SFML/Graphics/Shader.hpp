#ifndef SERIOUS_PROTON_SFML_OVER_SDL_SHADER_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_SHADER_HPP
#include <cstdint>
#include <string>
#include <SFML/Graphics/Glsl.hpp>
#include <SFML/System/InputStream.hpp>
#include <SFML/System/NonCopyable.hpp>
#include <glm/mat4x4.hpp>

#include "SDL_assert.h"
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
			SDL_assert_release(false && "not implemented");
		}

		// Non-SFML
		int32_t attribute(const char* name) const;
		explicit operator bool() const { return program != 0; }
	private:
		uint32_t vertexShader = 0;
		uint32_t fragmentShader = 0;
		uint32_t program = 0;
	};

	template<>
	void Shader::setUniform<glm::mat4>(const std::string&, const glm::mat4&);
	extern template void Shader::setUniform(const std::string &, const glm::mat4&);

	template<>
	void Shader::setUniform<Color>(const std::string&, const Color&);
	extern template void Shader::setUniform(const std::string&, const Color&);

	template<>
	void Shader::setUniform<bool>(const std::string&, const bool&);
	extern template void Shader::setUniform(const std::string&, const bool&);
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_SHADER_HPP