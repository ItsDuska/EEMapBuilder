#include "ChunkHandler.h"
#include <iostream>
#include <fstream>

#include "ChunkUtils.h"

constexpr int AMOUNT_OF_ALLOCATED_CHUNKS = 200;
constexpr int ANIMATED_TILES_ALLOCATION_ESTIMATE = 9 * 10 * 4;


chunk::ChunkHandler::ChunkHandler()
{
    chunks.reserve(AMOUNT_OF_ALLOCATED_CHUNKS);
    lastChunkCoord = { -1,-1 };
    loaded = false;
}


void chunk::ChunkHandler::update(sf::Vector2f& position)
{
    currentChunkCoord = { (int)std::floor((position.x / (CHUNK_SIZE * renderSizes.tileSize.x))),
        (int)std::floor((position.y / (CHUNK_SIZE * renderSizes.tileSize.y))) };

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

void chunk::ChunkHandler::addChunk(sf::Vector2i& chunkPosition)
{
    chunks.emplace_back();
    ChunkData* data = &chunks.back().rawData;
    data->x = static_cast<int16_t>(chunkPosition.x);
    data->y = static_cast<int16_t>(chunkPosition.y);
    activeChunks.push_back(chunkPosition);

    ChunkCreationTask task = {};
    task.chunkCoord = chunkPosition;
    task.hasTileMap = false;
    task.chunkData = data;
    chunkTaskList.push_back(task);

    ChunkKey key = combineCoords(chunkPosition.x, chunkPosition.y);
    chunkMap[key] = chunks.size()-1; 
}

void chunk::ChunkHandler::removeChunk(uint16_t index)
{
    if (index < activeChunks.size())
    {
        activeChunks.erase(activeChunks.begin() + index);
    }
}

void chunk::ChunkHandler::setAssetSizes(sf::Vector2f& tileSize, sf::Vector2f& textureSize,int sheetWidthInTiles)
{
    renderSizes.tileSize = tileSize;
    renderSizes.spritePixelSize = textureSize;
    renderSizes.totalChunkSize = {tileSize.x* CHUNK_SIZE,tileSize.y * CHUNK_SIZE };
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
                ChunkCreationTask task = {};
                task.chunkCoord = tempChunkCord;
                task.hasTileMap = true;
                task.chunkData = getChunkData(tempChunkCord.x, tempChunkCord.y);
                chunkTaskList.push_back(task);

                activeChunks.push_back(tempChunkCord);
              
            }
            else
            {
                addChunk(tempChunkCord);

            }
        }
    }

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

    std::vector<LayeredStaticTile> loadedLayers;
    loadedLayers.resize(sizes.layeredTiles);
    inFile.read(reinterpret_cast<char*>(loadedLayers.data()), sizeof(LayeredStaticTile) * sizes.layeredTiles);

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

        for (size_t indexToBuffer = chunk.layeredTilesBuffer.offset; indexToBuffer < chunk.layeredTilesBuffer.offset + chunk.layeredTilesBuffer.count; ++indexToBuffer)
        {
            editorChunk.layeredTiles.push_back(loadedLayers[indexToBuffer]);
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
        sizes.layeredTiles += chunk.layeredTiles.size();
    }

    std::vector<EntityTile> allEntities;
    allEntities.reserve(sizes.entities);

    std::vector<AnimationTile> allAnimations;
    allAnimations.reserve(sizes.animations);

    std::vector<LayeredStaticTile> allLayeredTiles;
    allLayeredTiles.reserve(sizes.layeredTiles);


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
                if (entity.animation.textureID != 0)
                {
                    filteredEntities.push_back(entity);
                }
            }

            chunk.rawData.entityBuffer.offset = currentSizes.entities;
            chunk.rawData.entityBuffer.count = filteredEntities.size();

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

            std::vector<LayeredStaticTile> filteredLayeredTiles;
            for (const auto& layeredTile : chunk.layeredTiles)
            {
                if (layeredTile.textureID != 0)
                {
                    filteredLayeredTiles.push_back(layeredTile);
                }
            }

            chunk.rawData.layeredTilesBuffer.offset = currentSizes.layeredTiles;
            chunk.rawData.layeredTilesBuffer.count = filteredLayeredTiles.size();

            outFile.write(reinterpret_cast<const char*>(&chunk.rawData), sizeof(ChunkData));

            allEntities.insert(allEntities.end(), filteredEntities.begin(), filteredEntities.end());
            allAnimations.insert(allAnimations.end(), filteredAnimations.begin(), filteredAnimations.end());
            allLayeredTiles.insert(allLayeredTiles.end(), filteredLayeredTiles.begin(), filteredLayeredTiles.end());

            currentSizes.entities += chunk.entities.size();
            currentSizes.animations += chunk.animations.size();
            currentSizes.layeredTiles += chunk.layeredTiles.size();
        }
        else
        {
            sizes.entities -= chunk.entities.size();
            sizes.animations -= chunk.animations.size();
            sizes.layeredTiles -= chunk.layeredTiles.size();
        }
    }

    outFile.write(reinterpret_cast<const char*>(allEntities.data()), sizeof(EntityTile) * sizes.entities);
    outFile.write(reinterpret_cast<const char*>(allAnimations.data()), sizeof(AnimationTile) * sizes.animations);
    outFile.write(reinterpret_cast<const char*>(allLayeredTiles.data()), sizeof(LayeredStaticTile)* sizes.layeredTiles);

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

ChunkData* chunk::ChunkHandler::getChunk(const sf::Vector2i& position)
{
    return this->getChunkData(position.x,position.y);
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

const chunk::RenderingSizes& chunk::ChunkHandler::getRenderSizes()
{
    return renderSizes;
}

std::vector<chunk::ChunkCreationTask>& chunk::ChunkHandler::getChunkCreationTasks()
{
    return chunkTaskList;
}

std::vector<uint16_t>& chunk::ChunkHandler::getDeletionQueue()
{
    return deletingChunksIndex;
}

// Determine if chunks have been loaded and trigger updates in other classes if needed.
bool chunk::ChunkHandler::needsUpdate()
{
    return !chunkTaskList.empty();
}

const std::vector<sf::Vector2i>& chunk::ChunkHandler::getActiveChunks()
{
    return activeChunks;

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
