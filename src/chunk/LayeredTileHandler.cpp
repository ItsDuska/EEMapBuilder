#include "LayeredTileHandler.h"
#include "chunk/ChunkHandler.h"

LayeredTileHandler::LayeredTileHandler(std::string& imageName, sf::Vector2f& spritePixelSize)
{
    vaTiles.setPrimitiveType(sf::Quads);

    if (!texture.loadFromFile(imageName))
    {
        std::cerr << "ERROR: Can't open texture!\n";
        return;
    }

    const sf::Vector2u imageSize = texture.getSize();

    spriteSheetSizeInTiles.x = imageSize.x / spritePixelSize.x;
    spriteSheetSizeInTiles.y = imageSize.y / spritePixelSize.y;

}

void LayeredTileHandler::render(sf::RenderTarget& window)
{
    window.draw(vaTiles, &texture);
}

void LayeredTileHandler::constractBuffer(chunk::ChunkHandler& handler)
{
    vaTiles.clear();

    sf::Vector2i texCoord(0, 0);
    const chunk::RenderingSizes& renderSizes = handler.getRenderSizes();

    for (const sf::Vector2i& chunkCoord : handler.getActiveChunks())
    {
        sf::Vector2i newPosition((renderSizes.totalChunkSize.x * chunkCoord.x) / renderSizes.tileSize.x, (renderSizes.totalChunkSize.y * chunkCoord.y) / renderSizes.tileSize.y);

        const chunk::EditorSideChunkData* data = handler.getEditorSideData(chunkCoord.x, chunkCoord.y);

        if (data->layeredTiles.size() == 0)
        {
            continue;
        }

        for (int i = 0; i < data->layeredTiles.size(); i++)
        {
            const LayeredStaticTile& tile = data->layeredTiles[i];

            if (tile.textureID == 0)
            {
                continue;
            }

            const int index = (tile.positionInChunk.y * CHUNK_SIZE + tile.positionInChunk.x);

            const int vertexIndex = (tile.positionInChunk.y * CHUNK_SIZE + tile.positionInChunk.x) * 4;
            sf::Vertex quad[4];

            const sf::Vector2i vertposition(newPosition.x + tile.positionInChunk.x, newPosition.y + tile.positionInChunk.y);

            //texCoord = animationCache.getAnimationFrame(tile.textureID, 0);

            texCoord.x = tile.textureID % spriteSheetSizeInTiles.x;
            texCoord.y = tile.textureID / spriteSheetSizeInTiles.x;

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

sf::Texture& LayeredTileHandler::getTexture()
{
    return texture;
}

sf::Vector2i& LayeredTileHandler::getSpriteSheetSizeInTiles()
{
    return spriteSheetSizeInTiles;
}