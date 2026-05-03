#pragma once
#include "animation/BaseAnimationHandler.h"
#include "core/types/Vector.hh"

class AnimationHandler : public BaseAnimationHandler {
public:
  AnimationHandler(std::string &animationFileName, Vec2 &spritePixelSize);
  void resetAnimationRandomness(chunk::ChunkHandler &handler);

  void constructTileBuffer(chunk::ChunkHandler &handler);
  void UpdateVATexCoords(chunk::ChunkHandler &handler);
};
