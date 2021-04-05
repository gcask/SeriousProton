#include "postProcessManager.h"
#include "logging.h"

#include <SFML/Graphics/Sprite.hpp>

bool PostProcessor::global_post_processor_enabled = true;

PostProcessor::PostProcessor(string name, RenderChain* chain)
: chain(chain)
{
    if (sf::Shader::isAvailable())
    {
        if (shader.loadFromFile("resources/" + name + ".frag", sf::Shader::Fragment))
        {
            LOG(INFO) << "Loaded shader: " << name;
            enabled = true;
        }else{
            LOG(WARNING) << "Failed to load shader:" << name;
            enabled = false;
        }
    }else{
        LOG(WARNING) << "Did not load load shader: " << name;
        LOG(WARNING) << "Because of no shader support in video card driver.";
        enabled = false;
    }
}

void PostProcessor::render(sf::RenderTarget& window)
{
    if (!enabled || !sf::Shader::isAvailable() || !global_post_processor_enabled)
    {
        chain->render(window);
        return;
    } 

    //Hack the rectangle for this element so it sits perfectly on pixel boundaries.
    sf::Vector2u pixel_size{ window.getSize() };

    // If the window or texture size is 0/impossible, or if the window size has
    // changed, resize the viewport, render texture, and input/textureSizes.
    if (size.x < 1 || renderTexture.getSize().x < 1 || size != pixel_size)
    {
        size = pixel_size;

        //Setup a backBuffer to render the game on. Then we can render the backbuffer back to the main screen with full-screen shader effects
        renderTexture.create(size.x, size.y, true);

        renderTexture.setRepeated(true);
        renderTexture.setSmooth(true);

        shader.setUniform("inputSize", sf::Vector2f{ size });
        shader.setUniform("textureSize", sf::Vector2f{ renderTexture.getSize() });
    }

    // The view can evolve independently from the window size - update every frame.
    renderTexture.setView(window.getView());
    renderTexture.clear(sf::Color(20, 20, 20));
   
    chain->render(renderTexture);

    renderTexture.display();

    // The RT is a fullscreen texture.
    // Setup the view to cover the entire RT.
    window.setView(sf::View({ 0.f, 0.f, static_cast<float>(renderTexture.getSize().x), static_cast<float>(renderTexture.getSize().y) }));
    
    sf::Sprite backBufferSprite(renderTexture.getTexture());
    window.draw(backBufferSprite, &shader);

    // Restore view
    window.setView(renderTexture.getView());
}

void PostProcessor::setUniform(string name, float value)
{
    if (sf::Shader::isAvailable() && global_post_processor_enabled)
        shader.setUniform(name, value);
}
