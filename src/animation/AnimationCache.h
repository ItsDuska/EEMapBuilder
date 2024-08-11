#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>
#include <iostream>


class AnimationCache
{
public:
    void awake(const sf::Vector2i frameSize, const sf::Vector2i textureSize);

    sf::Vector2i getAnimationFrame(int animationIndex, int frame) const;

    int getStartPosition(int index);
    int getMaxSprites();

    int getAnimationFrameCount(int animationIndex);

private:
    void precomputeStartPositions();

    std::vector<int> frameCounts;
    std::vector<sf::Vector2i> startPositions;
    sf::Vector2i frameSize;
    sf::Vector2i textureSize;
    bool built;
    int widthInTiles;
};
