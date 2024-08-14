#include "AnimationHandler.h"
#include "chunk/ChunkHandler.h"



/*

Tee uus main handler class joka omistaa kaiken mitäö chunkhandlerissa aiemmi oli

*/

AnimationHandler::AnimationHandler(std::string& animationFileName, sf::Vector2f& spritePixelSize)
{
	vaTiles.setPrimitiveType(sf::Quads);


    if (!texture.loadFromFile(animationFileName))
    {
        std::cerr << "ERROR: Can't open texture!\n";
        return;
    }

    const sf::Vector2u imageSize = texture.getSize();

    spriteSheetSizeInTiles.x = imageSize.x / spritePixelSize.x;
    spriteSheetSizeInTiles.y = imageSize.y / spritePixelSize.y;

    //spriteSheetSizeInTiles = {
        //static_cast<float>(texture.getSize().x) / spritePixelSize.x,
       // static_cast<float>(texture.getSize().y) / spritePixelSize.y
    //};


    animationCache.awake(
        sf::Vector2i(spritePixelSize.x, spritePixelSize.y),
        sf::Vector2i(texture.getSize().x, texture.getSize().y)
    );
    
}

void AnimationHandler::resetAnimationRandomness(chunk::ChunkHandler& handler)
{
    const int totalFrames = animationCache.getMaxSprites();

    for (const sf::Vector2i& chunkCoord : handler.getActiveChunks())
    {
        chunk::EditorSideChunkData* data = handler.getEditorSideData(chunkCoord.x, chunkCoord.y);

        if (data->animations.size() == 0)
        {
            continue;
        }

        for (int i = 0; i < data->animations.size(); i++)
        {
            AnimationTile& tile = data->animations[i];

            if (tile.textureID == 0)
            {
                continue;
            }

            tile.currentFrame = 0;
            tile.elapsedFrames = 0;
        }
    }
}

void AnimationHandler::render(sf::RenderTarget& window)
{
    window.draw(vaTiles, &texture);
}

void AnimationHandler::constrcuctAnimatedTiles(chunk::ChunkHandler& handler)
{
    vaTiles.clear();

    sf::Vector2i texCoord(0, 0);
    const chunk::RenderingSizes& renderSizes = handler.getRenderSizes();

    for (const sf::Vector2i& chunkCoord : handler.getActiveChunks())
    {
        sf::Vector2i newPosition((renderSizes.totalChunkSize.x * chunkCoord.x) / renderSizes.tileSize.x, (renderSizes.totalChunkSize.y * chunkCoord.y) / renderSizes.tileSize.y);

        const chunk::EditorSideChunkData* data = handler.getEditorSideData(chunkCoord.x, chunkCoord.y);

        if (data->animations.size() == 0)
        {
            continue;
        }

        for (int i = 0; i < data->animations.size(); i++)
        {
            const AnimationTile& tile = data->animations[i];

            if (tile.textureID == 0)
            {
                continue;
            }

            const int index = (tile.positionInChunk.y * CHUNK_SIZE + tile.positionInChunk.x);

            const int vertexIndex = (tile.positionInChunk.y * CHUNK_SIZE + tile.positionInChunk.x) * 4;
            sf::Vertex quad[4];

            const sf::Vector2i vertposition(newPosition.x + tile.positionInChunk.x, newPosition.y + tile.positionInChunk.y);

            texCoord = animationCache.getAnimationFrame(tile.textureID, 0);

            quad[0].position = { vertposition.x * renderSizes.tileSize.x ,vertposition.y * renderSizes.tileSize.y };
            quad[0].texCoords = { texCoord.x * renderSizes.spritePixelSize.x,texCoord.y * renderSizes.spritePixelSize.y };

            quad[1].position = { (vertposition.x + 1) * renderSizes.tileSize.x, vertposition.y * renderSizes.tileSize.y };
            quad[1].texCoords = { (texCoord.x + 1) * renderSizes.spritePixelSize.x,texCoord.y * renderSizes.spritePixelSize.y };

            quad[2].position = { (vertposition.x + 1) * renderSizes.tileSize.x, (vertposition.y + 1) * renderSizes.tileSize.y };
            quad[2].texCoords = { (texCoord.x + 1) * renderSizes.spritePixelSize.x, (texCoord.y + 1) * renderSizes.spritePixelSize.y };

            quad[3].position = { vertposition.x * renderSizes.tileSize.x, (vertposition.y + 1) * renderSizes.tileSize.y };
            quad[3].texCoords = { texCoord.x * renderSizes.spritePixelSize.x, (texCoord.y + 1) * renderSizes.spritePixelSize.y };

            for (int j = 0; j < 4; j++)
            {
                vaTiles.append(quad[j]);
            }
        }
    }
}

void AnimationHandler::UpdateVATexCoords(chunk::ChunkHandler& handler)
{
    int index = 0;
    int totalQuads = 0;

    const int totalFrames = animationCache.getMaxSprites();

    const chunk::RenderingSizes& renderSizes = handler.getRenderSizes();

    for (const sf::Vector2i& chunkCoord : handler.getActiveChunks())
    {
        chunk::EditorSideChunkData* data = handler.getEditorSideData(chunkCoord.x, chunkCoord.y);

        if (data->animations.size() == 0)
        {
            continue;
        }

        for (int i = 0; i < data->animations.size(); i++)
        {
            AnimationTile& tile = data->animations[i];

            if (tile.textureID == 0)
            {
                continue;
            }


            totalQuads++;

            sf::Vertex* quad = &vaTiles[index];
            sf::Vector2i texCoord = animationCache.getAnimationFrame(tile.textureID, tile.currentFrame);

            tile.elapsedFrames++;
            if (tile.elapsedFrames >= tile.frameDelay)
            {
                tile.elapsedFrames = 0;
                tile.currentFrame = (tile.currentFrame + 1) % animationCache.getAnimationFrameCount(tile.textureID);
            }

            quad[0].texCoords = { texCoord.x * renderSizes.spritePixelSize.x,texCoord.y * renderSizes.spritePixelSize.y };
            quad[1].texCoords = { (texCoord.x + 1) * renderSizes.spritePixelSize.x,texCoord.y * renderSizes.spritePixelSize.y };
            quad[2].texCoords = { (texCoord.x + 1) * renderSizes.spritePixelSize.x, (texCoord.y + 1) * renderSizes.spritePixelSize.y };
            quad[3].texCoords = { texCoord.x * renderSizes.spritePixelSize.x, (texCoord.y + 1) * renderSizes.spritePixelSize.y };

            index += 4;
        }
    }
}

sf::Texture& AnimationHandler::getTexture()
{
    return texture;
}

sf::Vector2i& AnimationHandler::getSpriteSheetSizeInTiles()
{
    return spriteSheetSizeInTiles;
}

AnimationCache& AnimationHandler::getAnimationCache()
{
    return animationCache;
}
