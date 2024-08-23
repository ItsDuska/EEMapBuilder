#pragma once
#include "animation/BaseAnimationHandler.h"



class AnimationHandler : public BaseAnimationHandler
{
public:
    AnimationHandler(std::string& animationFileName, sf::Vector2f& spritePixelSize);
    void resetAnimationRandomness(chunk::ChunkHandler& handler);
    
    void constructTileBuffer(chunk::ChunkHandler& handler);
    void UpdateVATexCoords(chunk::ChunkHandler& handler);
};
