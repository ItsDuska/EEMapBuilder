#pragma once
#include "AnimationCache.h"
#include "rendering/gpu/Buffer.hh"
#include <SDL3/SDL_gpu.h>
// #include <SFML/Graphics/VertexArray.hpp>
// #include <SFML/Graphics/Texture.hpp>

namespace chunk {
class ChunkHandler;
}

class BaseAnimationHandler {
public:
  BaseAnimationHandler(std::string &animationFileName, Vec2 &spritePixelSize);
  // void resetAnimationRandomness(chunk::ChunkHandler& handler);

  void render(SDL_GPURenderPass *renderpass);

  virtual void constructTileBuffer(chunk::ChunkHandler &handler) = 0;
  virtual void UpdateVATexCoords(chunk::ChunkHandler &handler) = 0;

  sf::Texture &getTexture();
  Vec2i &getSpriteSheetSizeInTiles();

  AnimationCache &getAnimationCache();

protected:
  AnimationCache animationCache;
  gpu::Buffer vertexBuffer;
  // sf::Texture texture;
  Vec2i spriteSheetSizeInTiles;
};
