#pragma once
#include "core/types/Vector.hh"

namespace gpu {
struct Vertex {
  Vec2 position;
  Vec2 texCoord;
  Color color;
};

struct LineVertex {
  Vec2 position;
  Color color;
};
} // namespace gpu
