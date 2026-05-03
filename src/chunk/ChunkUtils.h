#pragma once
#include <cstdint>

#include "rendering/gpu/Vertex.hh"

namespace chunk {
void addQuadVertices(gpu::Vertex *quad, const Vec2i &position,
                     const Vec2i &texCoord, const Vec2 &tileSize,
                     const Vec2 &textureSize, const bool isSolid);

bool isBitSet(uint32_t number, int n);

void setBit(uint32_t &number, int n);

void clearBit(uint32_t &number, int n);
} // namespace chunk
