#pragma once
#include "core/types/Vector.hh"
#include <string>
#include <vector>

class AnimationCache {
public:
  void awake(const Vec2i frameSize, const Vec2i textureSize,
             std::string &filepath);

  Vec2i getAnimationFrame(int animationIndex, int frame) const;
  std::vector<Vec2i> *getStartPositionsPtr();

  int getStartPosition(int index);
  int getMaxSprites();

  int getAnimationFrameCount(int animationIndex);

private:
  void precomputeStartPositions();

  std::vector<int> frameCounts;
  std::vector<Vec2i> startPositions;
  Vec2i frameSize;
  Vec2i textureSize;
  bool built;
  int widthInTiles;
};
