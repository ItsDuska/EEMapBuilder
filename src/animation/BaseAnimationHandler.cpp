#include "BaseAnimationHandler.h"
#include <SFML/Graphics/RenderTarget.hpp>

BaseAnimationHandler::BaseAnimationHandler(std::string& animationFileName, sf::Vector2f& spritePixelSize)
{
    vaTiles.setPrimitiveType(sf::Quads);

    std::string fullFilePath = animationFileName + ".png";

    if (!texture.loadFromFile(fullFilePath))
    {
        std::cerr << "ERROR: Can't open texture!\n";
        return;
    }

    const sf::Vector2u imageSize = texture.getSize();

    spriteSheetSizeInTiles.x = imageSize.x / spritePixelSize.x;
    spriteSheetSizeInTiles.y = imageSize.y / spritePixelSize.y;

    animationCache.awake(
        sf::Vector2i(spritePixelSize.x, spritePixelSize.y),
        sf::Vector2i(texture.getSize().x, texture.getSize().y),animationFileName
    );
}

void BaseAnimationHandler::render(sf::RenderTarget& window)
{
    window.draw(vaTiles, &texture);
}

sf::Texture& BaseAnimationHandler::getTexture()
{
    return texture;
}

sf::Vector2i& BaseAnimationHandler::getSpriteSheetSizeInTiles()
{
    return spriteSheetSizeInTiles;
}

AnimationCache& BaseAnimationHandler::getAnimationCache()
{
    return animationCache;
}
