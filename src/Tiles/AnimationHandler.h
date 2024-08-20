#pragma once
#include "animation/BaseAnimationHandler.h"



class AnimationHandler : public BaseAnimationHandler
{
public:
    AnimationHandler(std::string& animationFileName, sf::Vector2f& spritePixelSize);
    //AnimationHandler(std::string& animationFileName, sf::Vector2f& spritePixelSize);
    void resetAnimationRandomness(chunk::ChunkHandler& handler);

    //void render(sf::RenderTarget& window);
    
    void constrcuctAnimatedTiles(chunk::ChunkHandler& handler);
    void UpdateVATexCoords(chunk::ChunkHandler& handler);

    //sf::Texture& getTexture();
    //sf::Vector2i& getSpriteSheetSizeInTiles();

    //AnimationCache& getAnimationCache();
};
