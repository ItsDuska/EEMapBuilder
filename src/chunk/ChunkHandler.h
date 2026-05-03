#pragma once
#include "ChunkData.h"
#include "animation/AnimationCache.h"
#include <unordered_map>

namespace chunk {
using ChunkKey = int32_t;

constexpr int MAX_BUFFER_COUNT = 9;

struct EditorSideChunkData {
  ChunkData rawData;
  std::vector<EntityTile> entities;
  std::vector<AnimationTile> animations;
  std::vector<LayeredStaticTile> layeredTiles;
  EditorSideChunkData() : rawData(), entities(), animations(), layeredTiles() {}
};

struct BufferSizes {
  size_t chunks;
  size_t entities;
  size_t animations;
  size_t layeredTiles;
};

struct RenderingSizes {
  Vec2 tileSize;
  Vec2 spritePixelSize;
  Vec2 totalChunkSize;
};

struct ChunkCreationTask {
  Vec2i chunkCoord;
  bool hasTileMap;
  ChunkData *chunkData;
};

bool isChunkEmpty(const uint16_t *data, size_t size);

class ChunkHandler {
public:
  ChunkHandler();
  void update(Vec2 &position);
  void addChunk(Vec2i &chunkPosition);
  void removeChunk(uint16_t index);

  void setAssetSizes(Vec2 &tileSize, Vec2 &textureSize, int sheetWidthInTiles);

  void loadFromFile(const std::string &filename);
  void saveToFile(const std::string &filename);

  bool chunkInMemory(Vec2i &position);
  int chunkInActiveMemory(const Vec2i &position);

  ChunkData *getChunk(const Vec2i &position);
  const RenderingSizes &getRenderSizes();

  std::vector<ChunkCreationTask> &getChunkCreationTasks();
  std::vector<uint16_t> &getDeletionQueue();
  bool needsUpdate();

  const std::vector<Vec2i> &getActiveChunks();

  EditorSideChunkData *getEditorSideData(int16_t x, int16_t y);

private:
  void handleChunks();
  ChunkKey combineCoords(int16_t x, int16_t y);
  ChunkData *getChunkData(int16_t x, int16_t y);

private:
  std::vector<EditorSideChunkData> chunks;
  std::unordered_map<ChunkKey, uint16_t> chunkMap;
  std::vector<Vec2i> activeChunks;

  RenderingSizes renderSizes;
  Vec2i currentChunkCoord;
  Vec2i lastChunkCoord;
  bool loaded;
  // uusien chunkkien vbo:n luomiseen.
  std::vector<ChunkCreationTask> chunkTaskList;
  std::vector<uint16_t> deletingChunksIndex;
};
} // namespace chunk
