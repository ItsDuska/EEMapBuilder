#pragma once
#include "chunk/VBOHandler.h"
#include "animation/AnimationHandler.h"
#include "chunk/LayeredTileHandler.h"


struct EditorCreationInfo
{
	sf::Vector2f spritePixelSize;
	sf::Vector2f tileSize;
	std::string staticSpriteSheetTexturePath;
	std::string animatedSpriteSheetTexturePath;
	std::string objectSpriteSheetTexturePath;
	std::string layeredSpriteSheetTexturePath;

};

class EditorHandler
{
public:
	EditorHandler(EditorCreationInfo& info);

	void update(sf::Vector2f& position);
	void render(sf::RenderTarget& window,bool shaderUniformShowVisibility);


	VBOHandler& getVBOHandler();
	AnimationHandler& getAnimationHandler();
	chunk::ChunkHandler& getChunkHandler();
	LayeredTileHandler& getLayeredTileHandler();

	ChunkData* getChunkData(const sf::Vector2i& position);
	sf::VertexBuffer* getVBOPtr(const sf::Vector2i& position);



private:

	void updateBuffers();
private:
	chunk::ChunkHandler chunkHandler;
	VBOHandler vboHandler;
	AnimationHandler animationHandler;
	LayeredTileHandler layer;

};

