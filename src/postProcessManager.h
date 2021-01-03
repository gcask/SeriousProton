#ifndef POST_PROCESS_MANAGER_H
#define POST_PROCESS_MANAGER_H

#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/Vector2.hpp>

#include "stringImproved.h"
#include "Updatable.h"
#include "Renderable.h"

class PostProcessor : public RenderChain
{
private:
    sf::Shader shader;
    sf::RenderTexture renderTexture;
    sf::Vector2u size;
    
    RenderChain* chain;
    
    static bool global_post_processor_enabled;
public:
    bool enabled;
    
    PostProcessor(string name, RenderChain* chain);
    virtual ~PostProcessor() {}
    
    virtual void render(sf::RenderTarget& window);
    
    void setUniform(string name, float value);
    
    static void setEnable(bool enable) { global_post_processor_enabled = enable; }
    static bool isEnabled() { return global_post_processor_enabled; }
};

#endif//POST_PROCESS_MANAGER_H
