#pragma once
#include <SFML/Graphics.hpp>
#include "ChunkData.h"
#include <unordered_map>
#include "animation/AnimationCache.h"


namespace chunk
{
	using ChunkKey = int32_t;

	constexpr int MAX_BUFFER_COUNT = 9;

	struct EditorSideChunkData
	{
		ChunkData rawData;
		std::vector<EntityTile> entities;
		std::vector<AnimationTile> animations;
		std::vector<LayeredStaticTile> layeredTiles;
		EditorSideChunkData()
			: rawData(), entities(), animations(), layeredTiles()
		{
		}
	};

	struct BufferSizes
	{
		size_t chunks;
		size_t entities;
		size_t animations;
		size_t layeredTiles;
	};

	struct RenderingSizes
	{
		sf::Vector2f tileSize;
		sf::Vector2f spritePixelSize;
		sf::Vector2f totalChunkSize;
	};

	struct ChunkCreationTask
	{
		sf::Vector2i chunkCoord;
		bool hasTileMap;
		ChunkData* chunkData;
	};

	bool isChunkEmpty(const uint16_t* data, size_t size);

	class ChunkHandler
	{
	public:
		ChunkHandler();
		void update(sf::Vector2f& position);
		void addChunk(sf::Vector2i& chunkPosition);
		void removeChunk(uint16_t index);

		void setAssetSizes(
			sf::Vector2f& tileSize,
			sf::Vector2f& textureSize,
			int sheetWidthInTiles
		);

		void loadFromFile(const std::string& filename);
		void saveToFile(const std::string& filename);

		bool chunkInMemory(sf::Vector2i& position);
		int chunkInActiveMemory(const sf::Vector2i& position);

		ChunkData* getChunk(const sf::Vector2i& position);
		const RenderingSizes& getRenderSizes();

		std::vector<ChunkCreationTask>& getChunkCreationTasks();
		std::vector<uint16_t>& getDeletionQueue();
		bool needsUpdate();

		const std::vector<sf::Vector2i>& getActiveChunks();

		EditorSideChunkData* getEditorSideData(int16_t x, int16_t y);

	private:
		void handleChunks();
		ChunkKey combineCoords(int16_t x, int16_t y);
		ChunkData* getChunkData(int16_t x, int16_t y);
		
	private:
		std::vector<EditorSideChunkData> chunks;
		std::unordered_map<ChunkKey, uint16_t> chunkMap;
		std::vector<sf::Vector2i> activeChunks;

		RenderingSizes renderSizes;
		sf::Vector2i currentChunkCoord;
		sf::Vector2i lastChunkCoord;
		bool loaded;
		// uusien chunkkien vbo:n luomiseen.
		std::vector<ChunkCreationTask> chunkTaskList;
		std::vector<uint16_t> deletingChunksIndex;
	};
}