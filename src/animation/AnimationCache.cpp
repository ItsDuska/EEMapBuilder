#include "AnimationCache.h"

void AnimationCache::awake(const sf::Vector2i frameSize, const sf::Vector2i textureSize)
{
    if (built)
    {
        return;
    }

    frameCounts = { 0,6,2 }; // the fist value is NULL
    this->frameSize = frameSize;
    this->textureSize = textureSize;
    widthInTiles = textureSize.x / frameSize.x;
    built = true;

    precomputeStartPositions();
}

sf::Vector2i AnimationCache::getAnimationFrame(int animationIndex, int frame) const
{
    if (animationIndex == 0)
    {
        return sf::Vector2i(0, 0);
    }

    if (animationIndex >= startPositions.size())
    {
        std::cerr << "INFO: AnimationIndex to frames is too big... Your index is: " << animationIndex << "\n";
        animationIndex = 0;
    }

    sf::Vector2i startPos = startPositions[animationIndex];
    int frameCount = frameCounts[animationIndex]-1;

    if (frame == frameCount)
    {
        startPos.x += 1;

        if (startPos.x * frameSize.x > textureSize.x)
        {
            // Move to the next row
            startPos.x = 0;
            startPos.y += 1;
        }
    }

    return startPos;
}

int AnimationCache::getStartPosition(int index)
{
    if (index >= startPositions.size()) {
        //std::cerr << "guh? in animation buh\n";
        return 0;
    }
    sf::Vector2i position = startPositions[index];
    //std::cout << position.x << "x " << position.y << "y\n";
    int a = (position.y * widthInTiles + position.x);
    //std::cout << a << "\n";
    return a; // Palautetaan aloitusindeksi
}

int AnimationCache::getMaxSprites()
{
    return frameCounts.size();
}

int AnimationCache::getAnimationFrameCount(int animationIndex)
{
    if (animationIndex <= 0 || animationIndex >= frameCounts.size()) {
        std::cerr << "sussy Animation number: returning 0. Your number is " << animationIndex << "\n";
        return 0;
    }
    return frameCounts[animationIndex];
}

void AnimationCache::precomputeStartPositions()
{
    startPositions.push_back(sf::Vector2i(0, 0)); // NULL Value.

    int x = 16; 
    int y = 0;

    for (int i = 1; i < frameCounts.size(); ++i)
    {
        if (x + frameCounts[i] * frameSize.x > textureSize.x)
        {
            x = 1; // Aloita seuraavalta riviltä
            y += frameSize.y;
        }

        startPositions.push_back(sf::Vector2i(x/16, y/16));
        std::cout << i << " : values are |" << x/16 << "x " << y/16 << "y\n";
        x += frameCounts[i] * frameSize.x;
    }
}
