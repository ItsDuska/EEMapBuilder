#pragma once
#include "chunk/ChunkHandler.h"

// Class for handleling vertex buffer object for static tiles in the editor & chunk.
class VBOHandler
{
public:
    VBOHandler(std::string& animationFileName, sf::Vector2f& spritePixelSize);
    void addVertexBuffer(const sf::Vector2i& position, bool hasTileMap, ChunkData* chunkData, const chunk::RenderingSizes& renderSizes);
    void removeBuffer(const int index);
    
    void render(sf::RenderTarget& window, bool shaderUniformShowVisibility);

    sf::VertexBuffer* getBufferPtr(const int index);
    sf::Texture& getTexture();
    const sf::Vector2i& getSpriteSheetSizeInTiles();
private:
    std::vector<sf::VertexBuffer> buffers;
    sf::Texture texture;
    sf::Vector2i spriteSheetSizeInTiles;

    sf::RenderStates states;
    sf::Shader visibilityShader;
    
};

