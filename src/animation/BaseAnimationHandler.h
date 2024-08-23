#pragma once
#include "AnimationCache.h"
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace chunk
{
    class ChunkHandler;
}

class BaseAnimationHandler
{
public:
    BaseAnimationHandler(std::string& animationFileName, sf::Vector2f& spritePixelSize);
    //void resetAnimationRandomness(chunk::ChunkHandler& handler);

    void render(sf::RenderTarget& window);

    virtual void constructTileBuffer(chunk::ChunkHandler& handler) = 0;
    virtual void UpdateVATexCoords(chunk::ChunkHandler& handler) = 0;

    sf::Texture& getTexture();
    sf::Vector2i& getSpriteSheetSizeInTiles();

    AnimationCache& getAnimationCache();

protected:
    AnimationCache animationCache;
    sf::VertexArray vaTiles;
    sf::Texture texture;
    sf::Vector2i spriteSheetSizeInTiles;
};

