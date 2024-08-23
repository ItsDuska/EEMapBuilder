#include "Entity.h"
#include "chunk/ChunkHandler.h"

EntityHandler::EntityHandler(std::string& filepath, sf::Vector2f& spritePixelSize)
	: BaseAnimationHandler(filepath,spritePixelSize)
{
}

void EntityHandler::constructTileBuffer(chunk::ChunkHandler& handler)
{
    vaTiles.clear();

    sf::Vector2i texCoord(0, 0);
    const chunk::RenderingSizes& renderSizes = handler.getRenderSizes();

    for (const sf::Vector2i& chunkCoord : handler.getActiveChunks())
    {
        const sf::Vector2i newPosition((renderSizes.totalChunkSize.x * chunkCoord.x) / renderSizes.tileSize.x, (renderSizes.totalChunkSize.y * chunkCoord.y) / renderSizes.tileSize.y);

        const chunk::EditorSideChunkData* data = handler.getEditorSideData(chunkCoord.x, chunkCoord.y);

        if (data->entities.size() == 0)
        {
            continue;
        }

        for (int i = 0; i < data->entities.size(); i++)
        {
            const EntityTile& tile = data->entities[i];

            if (tile.animation.textureID == 0)
            {
                continue;
            }

            const int index = (tile.animation.positionInChunk.y * CHUNK_SIZE + tile.animation.positionInChunk.x);

            const int vertexIndex = (tile.animation.positionInChunk.y * CHUNK_SIZE + tile.animation.positionInChunk.x) * 4;
            sf::Vertex quad[4];

            const sf::Vector2i vertposition(newPosition.x + tile.animation.positionInChunk.x, newPosition.y + tile.animation.positionInChunk.y);

            texCoord = animationCache.getAnimationFrame(tile.animation.textureID, 0);

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

void EntityHandler::UpdateVATexCoords(chunk::ChunkHandler& handler)
{
    int index = 0;

    const int totalFrames = animationCache.getMaxSprites();

    const chunk::RenderingSizes& renderSizes = handler.getRenderSizes();

    for (const sf::Vector2i& chunkCoord : handler.getActiveChunks())
    {
        chunk::EditorSideChunkData* data = handler.getEditorSideData(chunkCoord.x, chunkCoord.y);

        if (data->entities.size() == 0)
        {
            continue;
        }

        for (int i = 0; i < data->animations.size(); i++)
        {
            EntityTile& tile = data->entities[i];

            const int frameCount = animationCache.getAnimationFrameCount(tile.animation.textureID);

            if (tile.animation.textureID == 0 || frameCount > 1)
            {
                continue;
            }

            sf::Vertex* quad = &vaTiles[index];
            sf::Vector2i texCoord = animationCache.getAnimationFrame(tile.animation.textureID, tile.animation.currentFrame);

            tile.animation.elapsedFrames++;
            if (tile.animation.elapsedFrames >= tile.animation.frameDelay)
            {
                tile.animation.elapsedFrames = 0;
                tile.animation.currentFrame = (tile.animation.currentFrame + 1) % frameCount;
            }

            quad[0].texCoords = { texCoord.x * renderSizes.spritePixelSize.x,texCoord.y * renderSizes.spritePixelSize.y };
            quad[1].texCoords = { (texCoord.x + 1) * renderSizes.spritePixelSize.x,texCoord.y * renderSizes.spritePixelSize.y };
            quad[2].texCoords = { (texCoord.x + 1) * renderSizes.spritePixelSize.x, (texCoord.y + 1) * renderSizes.spritePixelSize.y };
            quad[3].texCoords = { texCoord.x * renderSizes.spritePixelSize.x, (texCoord.y + 1) * renderSizes.spritePixelSize.y };

            index += 4;
        }
    }
}
