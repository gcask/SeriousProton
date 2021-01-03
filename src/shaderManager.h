#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H
#include "stringImproved.h"

namespace sf
{
    class Shader;
}

class ShaderInfo
{
public:
    sf::Shader* shader;
};

class ShaderManager
{
public:
    static sf::Shader* getShader(string name);

private:
    static std::map<string, ShaderInfo> shaders;
};

#endif//SHADER_MANAGER_H
