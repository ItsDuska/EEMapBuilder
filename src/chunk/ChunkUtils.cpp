#include "ChunkUtils.h"

void chunk::addQuadVertices(gpu::Vertex *quad, const Vec2i &position,
                            const Vec2i &texCoord, const Vec2 &tileSize,
                            const Vec2 &textureSize, const bool isSolid) {
  quad[0].position = Vec2(position.x * tileSize.x, position.y * tileSize.y);
  quad[1].position =
      Vec2((position.x + 1) * tileSize.x, position.y * tileSize.y);
  quad[2].position =
      Vec2((position.x + 1) * tileSize.x, (position.y + 1) * tileSize.y);
  quad[3].position =
      Vec2(position.x * tileSize.x, (position.y + 1) * tileSize.y);

  quad[0].texCoord =
      Vec2(texCoord.x * textureSize.x, texCoord.y * textureSize.y);
  quad[1].texCoord =
      Vec2((texCoord.x + 1) * textureSize.x, texCoord.y * textureSize.y);
  quad[2].texCoord =
      Vec2((texCoord.x + 1) * textureSize.x, (texCoord.y + 1) * textureSize.y);
  quad[3].texCoord =
      Vec2(texCoord.x * textureSize.x, (texCoord.y + 1) * textureSize.y);

  if (isSolid) {
    return;
  }

  Color color = {0, 0, 0, 0};
  quad[0].color = color;
  quad[1].color = color;
  quad[2].color = color;
  quad[3].color = color;
}

bool chunk::isBitSet(uint32_t number, int n) {
  return (number & (1 << n)) != 0;
}

void chunk::setBit(uint32_t &number, int n) { number |= (1 << n); }

void chunk::clearBit(uint32_t &number, int n) { number &= ~(1 << n); }
