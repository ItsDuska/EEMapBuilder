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
		EditorSideChunkData()
			: rawData(), entities(), animations()
		{
		}
	};

	struct BufferSizes
	{
		size_t chunks;
		size_t entities;
		size_t animations;
	};


	void addQuadVertices(sf::Vertex* quad, const sf::Vector2i& position, const sf::Vector2i& texCoord, sf::Vector2f& tileSize,
		sf::Vector2f& textureSize,bool isSolid);

	bool isBitSet(uint32_t number, int n);
	void setBit(uint32_t& number, int n);
	void clearBit(uint32_t& number, int n);

	bool isChunkEmpty(const uint16_t* data, size_t size);


	class ChunkHandler
	{
	public:
		ChunkHandler();
		void update(sf::Vector2f& position);
		void addChunk(sf::Vector2i chunkPosition);
		void removeChunk(uint16_t index);
		void setAssetSizes(sf::Vector2f& tileSize,
			sf::Vector2f& textureSize,
			int sheetWidthInTiles,
			sf::Vector2i& animatedTextureSize);

		void renderActiveChunks(sf::RenderTarget& window,sf::RenderStates& states, sf::RenderStates& animatedTiles);

		sf::VertexBuffer* getBuffer(const sf::Vector2i& position);
		ChunkData* getChunk(const sf::Vector2i& position);
		sf::Vector2i getAnimatedTextureCoord(int index);
		std::vector<AnimationTile>& getAnimationTileDataBuffer(int16_t x, int16_t y);
		int getAnimationCacheStartingIndex(int index);
		int getAnimationCacheMaxSprites();

		AnimationCache& getAnimationCache();

		void constrcuctAnimatedTiles();
		void UpdateVATexCoords();
		void resetAnimationRandomness();

		void loadFromFile(const std::string& filename);
		void saveToFile(const std::string& filename);
	public:

	private:
		void handleChunks();

		ChunkKey combineCoords(int16_t x, int16_t y);
		ChunkData* getChunkData(int16_t x, int16_t y);
		EditorSideChunkData* getEditorSideData(int16_t x, int16_t y);

		uint16_t getChunkBufferIndex(sf::Vector2i& position);
		
		bool chunkInMemory(sf::Vector2i& position);
		int chunkInActiveMemory(const sf::Vector2i& position);

		void addVertexBuffer(sf::Vector2i& position, bool hasTileMap);


		


	private:
		std::vector<EditorSideChunkData> chunks;
		std::vector<sf::VertexBuffer> vertexBuffers;
		std::unordered_map<ChunkKey, uint16_t> chunkMap;
		std::vector<sf::Vector2i> activeChunks;

		sf::VertexArray animatedTiles;

		sf::Vector2f tileSize;
		sf::Vector2f textureSize;

		sf::Vector2i currentChunkCoord;
		sf::Vector2i lastChunkCoord;
		sf::Vector2f chunkSize;
		int sheetWidthInTiles;

		bool loaded;

		AnimationCache animationCache;


	};

}