#include "VBOHandler.h"
#include "chunk/ChunkUtils.h"


constexpr int MAX_BUFFER_COUNT = 9;


VBOHandler::VBOHandler(std::string& animationFileName, sf::Vector2f& spritePixelSize)
{
    buffers.reserve(MAX_BUFFER_COUNT);

    if (!texture.loadFromFile(animationFileName))
    {
        std::cerr << "ERROR: Can't open texture!\n";
        return;
    }

    const sf::Vector2u imageSize = texture.getSize();

    spriteSheetSizeInTiles.x = imageSize.x / spritePixelSize.x;
    spriteSheetSizeInTiles.y = imageSize.y / spritePixelSize.y;

    
    if (!visibilityShader.loadFromFile("src/shaders/shader.vert", "src/shaders/shader.frag"))
    {
        std::cerr << "ERROR: Can't load shaders!\n";
    }

    states.shader = &visibilityShader;
    states.texture = &texture;
}

void VBOHandler::addVertexBuffer(const sf::Vector2i& position, bool hasTileMap, ChunkData* chunkData, const chunk::RenderingSizes& renderSizes)
{
    sf::VertexBuffer buffer;

    const uint32_t vertexCount = TILE_MAP_SIZE * 4;
    buffer.create(vertexCount);
    buffer.setPrimitiveType(sf::Quads);
    buffer.setUsage(sf::VertexBuffer::Static);

    std::vector<sf::Vertex> vertexArray(vertexCount);

    sf::Vector2i texCoord(0, 0);
    sf::Vector2i newPosition(
        (renderSizes.totalChunkSize.x * position.x) / renderSizes.tileSize.x,
        (renderSizes.totalChunkSize.y * position.y) / renderSizes.tileSize.y
    );

    for (int y = 0; y < CHUNK_SIZE; ++y)
    {
        for (int x = 0; x < CHUNK_SIZE; ++x)
        {
            const int index = y * CHUNK_SIZE + x;
            const int vertexIndex = index * 4;
            sf::Vertex* quad = &vertexArray[vertexIndex];
            const sf::Vector2i vertPosition(newPosition.x + x, newPosition.y + y);

            if (hasTileMap)
            {
                if (chunkData == nullptr)
                {
                    std::cerr << "Error: ChunkData is nullptr\n";
                    return;
                }

                const uint16_t textureIndex = chunkData->tilemap[index];
                texCoord.x = textureIndex % spriteSheetSizeInTiles.x;
                texCoord.y = textureIndex / spriteSheetSizeInTiles.x;

                bool isSolid = chunk::isBitSet(chunkData->solidBlockData[y], x);
                chunk::addQuadVertices(quad, vertPosition, texCoord, renderSizes.tileSize, renderSizes.spritePixelSize, isSolid);
            }
            else
            {
                chunk::addQuadVertices(quad, vertPosition, texCoord, renderSizes.tileSize, renderSizes.spritePixelSize, false);
            }
        }
    }

    buffer.update(vertexArray.data());
    buffers.push_back(std::move(buffer));
}

sf::VertexBuffer* VBOHandler::getBufferPtr(const int index)
{
    return &buffers[index];
}

sf::Texture& VBOHandler::getTexture()
{
    return texture;
}

void VBOHandler::render(sf::RenderTarget& window, bool shaderUniformShowVisibility)
{
    visibilityShader.setUniform("solidBlockVisibility", static_cast<float>(shaderUniformShowVisibility));
    
    for (const sf::VertexBuffer& buffer : buffers)
    {
        window.draw(buffer, states);
    }
}

void VBOHandler::removeBuffer(const int index)
{
    buffers.erase(buffers.begin() + index);
}

const sf::Vector2i& VBOHandler::getSpriteSheetSizeInTiles()
{
    return spriteSheetSizeInTiles;
}

