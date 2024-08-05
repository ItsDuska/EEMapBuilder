#include "ChunkHandler.h"
#include <iostream>
#include <fstream>

constexpr int AMOUNT_OF_ALLOCATED_CHUNKS = 200;
constexpr int ANIMATED_TILES_ALLOCATION_ESTIMATE = 9 * 10 * 4;


chunk::ChunkHandler::ChunkHandler()
{
    chunks.reserve(AMOUNT_OF_ALLOCATED_CHUNKS);
    vertexBuffers.reserve(MAX_BUFFER_COUNT);

    lastChunkCoord = { -1,-1 };
    loaded = false;
    sheetWidthInTiles = 0;

    animatedTiles.setPrimitiveType(sf::Quads);
    //animatedTiles.resize(ANIMATED_TILES_ALLOCATION_ESTIMATE);
    

    //animationCache.awake({16,16},{tex})
}


void chunk::ChunkHandler::update(sf::Vector2f& position)
{
    currentChunkCoord = { (int)std::floor((position.x / (CHUNK_SIZE * tileSize.x))),
        (int)std::floor((position.y / (CHUNK_SIZE * tileSize.y))) };

    if (lastChunkCoord != currentChunkCoord)
    {
        if (!loaded)
        {
            std::cout << "INFO: Loading new chunks\n";
            handleChunks();
        }
    }
    else
    {
        loaded = false;
    }

    lastChunkCoord = currentChunkCoord;
}

void chunk::ChunkHandler::addChunk(sf::Vector2i chunkPosition)
{
    chunks.emplace_back();
    ChunkData* data = &chunks.back().rawData;
    data->x = static_cast<int16_t>(chunkPosition.x);
    data->y = static_cast<int16_t>(chunkPosition.y);
    
    addVertexBuffer(chunkPosition, false);
    ChunkKey key = combineCoords(chunkPosition.x, chunkPosition.y);
    chunkMap[key] = chunks.size()-1; 
}

void chunk::ChunkHandler::removeChunk(uint16_t index)
{
    if (index < activeChunks.size())
    {
        activeChunks.erase(activeChunks.begin() + index);
        vertexBuffers.erase(vertexBuffers.begin() + index);
    }
}

void chunk::ChunkHandler::setAssetSizes(sf::Vector2f& tileSize, sf::Vector2f& textureSize,int sheetWidthInTiles, sf::Vector2i& animatedTextureSize)
{
    this->tileSize = tileSize;
    this->textureSize = textureSize;
    this->sheetWidthInTiles = sheetWidthInTiles;
    chunkSize = {tileSize.x* CHUNK_SIZE,tileSize.y * CHUNK_SIZE };
    animationCache.awake({ 16,16 }, animatedTextureSize);
}

void chunk::ChunkHandler::renderActiveChunks(sf::RenderTarget& window, sf::RenderStates& states, sf::RenderStates& animatedTileState)
{
    for (const sf::VertexBuffer& buffer : vertexBuffers)
    {
        window.draw(buffer, states); 
    }

    window.draw(animatedTiles, animatedTileState);
    //window.draw(animatedTiles, states);
}

void chunk::ChunkHandler::handleChunks()
{
    std::vector<sf::Vector2i> loadingCoords;
    sf::Vector2i tempChunkCord;

    const int renderBonds = 3;
    const int halfRenderBonds = 2;

    loadingCoords.reserve(renderBonds * renderBonds);

    for (int x = 0; x < renderBonds; x++)
    {
        for (int y = 0; y < renderBonds; y++)
        {
            tempChunkCord = {
                    (x + 1) - halfRenderBonds + currentChunkCoord.x,
                    (y + 1) - halfRenderBonds + currentChunkCoord.y
            };

            loadingCoords.push_back(tempChunkCord);

            if (chunkInActiveMemory(tempChunkCord) != -1)
            {
                continue;
            }

            if (chunkInMemory(tempChunkCord))
            {
                addVertexBuffer(tempChunkCord, true);
            }
            else
            {
                addChunk(tempChunkCord);
            }
        }
    }
    std::vector<uint16_t> deletingChunksIndex;
    deletingChunksIndex.reserve(activeChunks.size());

    for (uint16_t i = 0; i < activeChunks.size(); i++)
    {
        if (std::find(loadingCoords.begin(), loadingCoords.end(),
            activeChunks[i]) == loadingCoords.end())
        {
            deletingChunksIndex.push_back(i);
        }
    }

    for (uint16_t index = 0; index < deletingChunksIndex.size(); index++)
    {
        const uint16_t fixedIndex = deletingChunksIndex[index] - index;
        this->removeChunk(fixedIndex);
    }

    this->loaded = true;

    constrcuctAnimatedTiles();
}

void chunk::ChunkHandler::loadFromFile(const std::string& filename)
{
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile)
    {
        std::cerr << "ERROR: Cannot open file for reading!\n";
        return;
    }

    BufferSizes sizes{};
    inFile.read(reinterpret_cast<char*>(&sizes), sizeof(BufferSizes));

    std::vector<ChunkData> loadedChunks;
    loadedChunks.resize(sizes.chunks);
    inFile.read(reinterpret_cast<char*>(loadedChunks.data()), sizeof(ChunkData) * sizes.chunks);

    std::vector<EntityTile> loadedEntities;
    loadedEntities.resize(sizes.entities);
    inFile.read(reinterpret_cast<char*>(loadedEntities.data()), sizeof(EntityTile) * sizes.entities);

    std::vector<AnimationTile> loadedAnimations;
    loadedAnimations.resize(sizes.animations);
    inFile.read(reinterpret_cast<char*>(loadedAnimations.data()), sizeof(AnimationTile) * sizes.animations);

    if (chunks.capacity() < sizes.chunks)
    {
        chunks.resize(sizes.chunks);
    }

    for (size_t i = 0; i < sizes.chunks; ++i)
    {
        ChunkData& chunk = loadedChunks[i];
        EditorSideChunkData editorChunk;
        editorChunk.rawData = chunk;

        for (size_t indexToBuffer = chunk.entityBuffer.offset; indexToBuffer < chunk.entityBuffer.offset + chunk.entityBuffer.count; ++indexToBuffer)
        {
            editorChunk.entities.push_back(loadedEntities[indexToBuffer]);
        }

        for (size_t indexToBuffer = chunk.animatedTileBuffer.offset; indexToBuffer < chunk.animatedTileBuffer.offset + chunk.animatedTileBuffer.count; ++indexToBuffer)
        {
            editorChunk.animations.push_back(loadedAnimations[indexToBuffer]);
        }

        chunkMap[combineCoords(chunk.x, chunk.y)] = i;
        chunks.push_back(editorChunk);
    }

    inFile.close();
}

void chunk::ChunkHandler::saveToFile(const std::string& filename)
{
    if (chunks.empty())
    {
        std::cerr << "ERROR: Chunks are empty somehow while saving!\n";
        return;
    }

    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile)
    {
        std::cerr << "ERROR: Cannot open file for writing!\n";
        return;
    }

    BufferSizes sizes{};
    outFile.seekp(sizeof(BufferSizes)); // byte offset


    //calculate the max amount of entities for the buffer that could be saved
    for (const auto& entry : chunkMap)
    {
        EditorSideChunkData& chunk = chunks[entry.second];
        sizes.entities += chunk.entities.size();
        sizes.animations += chunk.animations.size();
    }

    std::vector<EntityTile> allEntities;
    allEntities.reserve(sizes.entities);

    std::vector<AnimationTile> allAnimations;
    allAnimations.reserve(sizes.animations);


    BufferSizes currentSizes{};


    for (const auto& entry : chunkMap)
    {
        EditorSideChunkData& chunk = chunks[entry.second];
        if (!isChunkEmpty(chunk.rawData.tilemap, TILE_MAP_SIZE))
        {
            sizes.chunks++;
            
            std::vector<EntityTile> filteredEntities;
            for (const auto& entity : chunk.entities)
            {
                if (entity.textureID != 0)
                {
                    filteredEntities.push_back(entity);
                }
            }

            chunk.rawData.animatedTileBuffer.offset = currentSizes.animations;
            chunk.rawData.animatedTileBuffer.count = filteredEntities.size();

            std::vector<AnimationTile> filteredAnimations;
            for (const auto& animation : chunk.animations)
            {
                if (animation.textureID != 0)
                {
                    filteredAnimations.push_back(animation);
                }
            }

            chunk.rawData.animatedTileBuffer.offset = currentSizes.animations;
            chunk.rawData.animatedTileBuffer.count = filteredAnimations.size();

            outFile.write(reinterpret_cast<const char*>(&chunk.rawData), sizeof(ChunkData));

            allEntities.insert(allEntities.end(), filteredEntities.begin(), filteredEntities.end());
            allAnimations.insert(allAnimations.end(), filteredAnimations.begin(), filteredAnimations.end());

            currentSizes.entities += chunk.entities.size();
            currentSizes.animations += chunk.animations.size();
        }
        else
        {
            sizes.entities -= chunk.entities.size();
            sizes.animations -= chunk.animations.size();
        }
    }

    outFile.write(reinterpret_cast<const char*>(allEntities.data()), sizeof(EntityTile) * sizes.entities);
    outFile.write(reinterpret_cast<const char*>(allAnimations.data()), sizeof(AnimationTile) * sizes.animations);

    outFile.seekp(0);
    outFile.write(reinterpret_cast<const char*>(&sizes), sizeof(BufferSizes));
    outFile.close();
}

chunk::ChunkKey chunk::ChunkHandler::combineCoords(int16_t x, int16_t y)
{
    return (static_cast<uint32_t>(x) << 16) | (static_cast<uint32_t>(y) & 0xFFFF);
}

ChunkData* chunk::ChunkHandler::getChunkData(int16_t x, int16_t y)
{
    ChunkKey key = combineCoords(x, y);
    auto it = chunkMap.find(key);

    if (it != chunkMap.end())
    {
        return &chunks[it->second].rawData;
    }

    return nullptr;
}

chunk::EditorSideChunkData* chunk::ChunkHandler::getEditorSideData(int16_t x, int16_t y)
{
    ChunkKey key = combineCoords(x, y);
    auto it = chunkMap.find(key);

    if (it != chunkMap.end())
    {
        return &chunks[it->second];
    }

    return nullptr;
}

uint16_t chunk::ChunkHandler::getChunkBufferIndex(sf::Vector2i& position)
{
    return chunkMap.find(combineCoords(position.x, position.y))->second;
}

int chunk::ChunkHandler::getAnimationCacheStartingIndex(int index)
{
    return animationCache.getStartPosition(index);
}

int chunk::ChunkHandler::getAnimationCacheMaxSprites()
{
    return animationCache.getMaxSprites();
}

sf::VertexBuffer* chunk::ChunkHandler::getBuffer(const sf::Vector2i& position)
{
    return &vertexBuffers[chunkInActiveMemory(position)];
}

ChunkData* chunk::ChunkHandler::getChunk(const sf::Vector2i& position)
{
    return this->getChunkData(position.x,position.y);
}

sf::Vector2i chunk::ChunkHandler::getAnimatedTextureCoord(int index)
{
    return animationCache.getAnimationFrame(index, 0);
}

std::vector<AnimationTile>& chunk::ChunkHandler::getAnimationTileDataBuffer(int16_t x, int16_t y)
{
    return getEditorSideData(x, y)->animations;
}

bool chunk::ChunkHandler::chunkInMemory(sf::Vector2i& position)
{
    const auto result = chunkMap.find(combineCoords(position.x, position.y));
    return result != chunkMap.end();
}

int chunk::ChunkHandler::chunkInActiveMemory(const sf::Vector2i& position)
{
    std::vector<sf::Vector2i>::iterator iteratorObj = std::find(activeChunks.begin(), activeChunks.end(), position);

    if (iteratorObj != activeChunks.end())
    {
        return static_cast<int>((iteratorObj - activeChunks.begin()));
    }
    else
    {
        return -1;
    }
}

void chunk::ChunkHandler::addVertexBuffer(sf::Vector2i& position, bool hasTileMap)
{
    activeChunks.push_back(position);
    
    sf::VertexBuffer buffer;
    const uint32_t vertexSize = TILE_MAP_SIZE * 4;
    
    buffer.create(vertexSize);
    buffer.setPrimitiveType(sf::Quads);
    buffer.setUsage(sf::VertexBuffer::Static);

    std::vector<sf::Vertex> vertexArray(vertexSize);
    sf::Vector2i texCoord(0, 0);
    sf::Vector2i newPosition((chunkSize.x * position.x)/tileSize.x, (chunkSize.y * position.y)/tileSize.y);
   
    if (!hasTileMap)
    {
        for (int y = 0; y < CHUNK_SIZE; y++)
        {
            for (int x = 0; x < CHUNK_SIZE; x++)
            {
                const int index = (y * CHUNK_SIZE + x) * 4;
                sf::Vertex* quad = &vertexArray[index];
                const sf::Vector2i vertposition(newPosition.x + x, newPosition.y + y);

                addQuadVertices(quad, vertposition, texCoord, tileSize, textureSize,false);
            }
        }
    }
    else
    {
        ChunkData* data = this->getChunkData(position.x, position.y);

        if (data == nullptr)
        {
            std::cout << "huh???\n";
            return;
        }

        for (int y = 0; y < CHUNK_SIZE; y++)
        {
            for (int x = 0; x < CHUNK_SIZE; x++)
            {
                const int index = (y * CHUNK_SIZE + x);
                const int vertexIndex = index * 4;
                sf::Vertex* quad = &vertexArray[vertexIndex];
                const sf::Vector2i vertposition(newPosition.x + x, newPosition.y + y);
                const uint16_t textureIndex = data->tilemap[index];
                if (textureIndex != 0)
                {
                    texCoord.x = textureIndex % sheetWidthInTiles;
                    texCoord.y = textureIndex / sheetWidthInTiles;
                }
                else
                {
                    texCoord.x = 0;
                    texCoord.y = 0;
                }

                bool isSolid = isBitSet(data->solidBlockData[y],x);

                addQuadVertices(quad, vertposition, texCoord, tileSize, textureSize,isSolid);
            }
        }
    }

    buffer.update(vertexArray.data());
    vertexBuffers.push_back(buffer);
}

void chunk::ChunkHandler::constrcuctAnimatedTiles()
{
    animatedTiles.clear();

    sf::Vector2i texCoord(0, 0);

    //std::cout << textureSize.x << "x | " << textureSize.x << "y\n";


    for (const sf::Vector2i& chunkCoord : activeChunks)
    {
        sf::Vector2i newPosition((chunkSize.x * chunkCoord.x) / tileSize.x, (chunkSize.y * chunkCoord.y) / tileSize.y);

        const EditorSideChunkData* data = getEditorSideData(chunkCoord.x, chunkCoord.y);
        
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
            //sf::Vertex* quad = &animatedTiles[vertexIndex];
            sf::Vertex quad[4];

            const sf::Vector2i vertposition(newPosition.x + tile.positionInChunk.x, newPosition.y + tile.positionInChunk.y);

            //std::cout << "TextureID at construct: " << tile.textureID << "\n";

            //tile.textureID
            texCoord = animationCache.getAnimationFrame(tile.textureID, 0);
            //addQuadVertices(quad, vertposition, texCoord, tileSize, textureSize, false);



            quad[0].position = { vertposition.x * tileSize.x ,vertposition.y * tileSize.y };
            quad[0].texCoords = {texCoord.x * textureSize.x,texCoord.y * textureSize.y };

           // std::cout << quad[0].texCoords.x << "x | " << quad[0].texCoords.x << "y\n";

            quad[1].position = { (vertposition.x+1) * tileSize.x, vertposition.y* tileSize.y };
            quad[1].texCoords = { (texCoord.x+1) * textureSize.x,texCoord.y * textureSize.y };

           // std::cout << quad[1].texCoords.x << "x | " << quad[1].texCoords.x << "y\n";

            quad[2].position = { (vertposition.x + 1) * tileSize.x, (vertposition.y + 1) * tileSize.y };
            quad[2].texCoords = { (texCoord.x + 1) * textureSize.x, (texCoord.y + 1) * textureSize.y };

           // std::cout << quad[2].texCoords.x << "x | " << quad[2].texCoords.x << "y\n";

            quad[3].position = { vertposition.x * tileSize.x, (vertposition.y + 1) * tileSize.y };
            quad[3].texCoords = { texCoord.x * textureSize.x, (texCoord.y + 1) * textureSize.y };

           // std::cout << quad[3].texCoords.x << "x | " << quad[3].texCoords.x << "y\n";



            for (int j = 0; j < 4; j++)
            {
                animatedTiles.append(quad[j]);
            }
            
        }

    }
    
}


/*
TÄÄÄÄ TUHOO SUN KONEEN OIKEESTI PLSSS FIXAAAA!!! TÄTÄ SOFTAA EI VOI SULKEA TASK MANAGERILLA
*/
void chunk::ChunkHandler::UpdateVATexCoords()
{
    int index = 0;
    int totalQuads = 0;
    //return; //!!!!!!!!!!!!!!!!!!!
    //std::cout << "STARTING BATCHING\n";

    const int totalFrames = getAnimationCacheMaxSprites();

    for (const sf::Vector2i& chunkCoord : activeChunks) {
        EditorSideChunkData* data = getEditorSideData(chunkCoord.x, chunkCoord.y);

        if (data->animations.size() == 0)
        {
            continue;
        }

        for (int i = 0; i < data->animations.size(); i++) {
            AnimationTile& tile = data->animations[i];

            // Päivitä animaation frame
            
            if (tile.textureID == 0)
            {
                continue;
            }
           

            totalQuads++;

            sf::Vertex* quad = &animatedTiles[index];
            sf::Vector2i texCoord = animationCache.getAnimationFrame(tile.textureID, tile.currentFrame);

            tile.elapsedFrames++;
            if (tile.elapsedFrames >= tile.frameDelay)
            {
                tile.elapsedFrames = 0;
                tile.currentFrame = (tile.currentFrame + 1) % animationCache.getAnimationFrameCount(tile.textureID);
            }

           

            // Aseta tekstuurikoordinaatit

            quad[0].texCoords = { texCoord.x * textureSize.x,texCoord.y * textureSize.y };
            quad[1].texCoords = { (texCoord.x + 1) * textureSize.x,texCoord.y * textureSize.y };  
            quad[2].texCoords = { (texCoord.x + 1) * textureSize.x, (texCoord.y + 1) * textureSize.y }; 
            quad[3].texCoords = { texCoord.x * textureSize.x, (texCoord.y + 1) * textureSize.y };

            //quad[0].texCoords = { static_cast<float>(texCoord.x * textureSize.x), static_cast<float>(texCoord.y * textureSize.y) };
            //quad[1].texCoords = { static_cast<float>((texCoord.x + 1) * textureSize.x), static_cast<float>(texCoord.y * textureSize.y) };
            //quad[2].texCoords = { static_cast<float>((texCoord.x + 1) * textureSize.x), static_cast<float>((texCoord.y + 1) * textureSize.y) };
            //quad[3].texCoords = { static_cast<float>(texCoord.x * textureSize.x), static_cast<float>((texCoord.y + 1) * textureSize.y) };

            index += 4;
        }
    }

    //std::cout << "ENDING BATCHING\n";
}


void chunk::addQuadVertices(sf::Vertex* quad, const sf::Vector2i& position, const sf::Vector2i& texCoord, sf::Vector2f& tileSize, sf::Vector2f& textureSize, bool isSolid)
{
    quad[0].position = sf::Vector2f(position.x * tileSize.x, position.y * tileSize.y);
    quad[1].position = sf::Vector2f((position.x + 1) * tileSize.x, position.y * tileSize.y);
    quad[2].position = sf::Vector2f((position.x + 1) * tileSize.x, (position.y + 1) * tileSize.y);
    quad[3].position = sf::Vector2f(position.x * tileSize.x, (position.y + 1) * tileSize.y);

    quad[0].texCoords = sf::Vector2f(texCoord.x * textureSize.x, texCoord.y * textureSize.y);
    quad[1].texCoords = sf::Vector2f((texCoord.x + 1) * textureSize.x, texCoord.y * textureSize.y);
    quad[2].texCoords = sf::Vector2f((texCoord.x + 1) * textureSize.x, (texCoord.y + 1) * textureSize.y);
    quad[3].texCoords = sf::Vector2f(texCoord.x * textureSize.x, (texCoord.y + 1) * textureSize.y);

    if (isSolid)
    {
        return;
    }

    sf::Color color = sf::Color::Black;
    quad[0].color = color;
    quad[1].color = color;
    quad[2].color = color;
    quad[3].color = color;
}

bool chunk::isBitSet(uint32_t number, int n)
{
    return (number & (1 << n)) != 0;
}

void chunk::setBit(uint32_t& number, int n)
{
    number |= (1 << n);
}

void chunk::clearBit(uint32_t& number, int n)
{
    number &= ~(1 << n);
}

bool chunk::isChunkEmpty(const uint16_t* data, size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        if (data[i] != 0)
        {
            return false;
        }
    }
    return true;
}
