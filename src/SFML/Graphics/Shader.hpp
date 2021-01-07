#ifndef SERIOUS_PROTON_SFML_OVER_SDL_SHADER_HPP
#define	SERIOUS_PROTON_SFML_OVER_SDL_SHADER_HPP
#include <cstdint>
#include <string>
#include <SFML/Graphics/Glsl.hpp>
#include <SFML/System/InputStream.hpp>
#include <SFML/System/NonCopyable.hpp>
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

		}

	};
}
#endif // SERIOUS_PROTON_SFML_OVER_SDL_SHADER_HPP