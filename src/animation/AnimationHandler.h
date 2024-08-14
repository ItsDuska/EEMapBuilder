#pragma once
#include "AnimationCache.h"
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Texture.hpp>
//#include <string>

namespace chunk
{
    class ChunkHandler;
}


class AnimationHandler
{
public:
    AnimationHandler(std::string& animationFileName, sf::Vector2f& spritePixelSize);
    void resetAnimationRandomness(chunk::ChunkHandler& handler);

    void render(sf::RenderTarget& window);
    
    void constrcuctAnimatedTiles(chunk::ChunkHandler& handler);
    void UpdateVATexCoords(chunk::ChunkHandler& handler);

    sf::Texture& getTexture();
    sf::Vector2i& getSpriteSheetSizeInTiles();

    AnimationCache& getAnimationCache();

private:
    AnimationCache animationCache;
    sf::VertexArray vaTiles;
    sf::Texture texture;
    sf::Vector2i spriteSheetSizeInTiles;
};
