#include "WorldHandler.h"

EditorHandler::EditorHandler(EditorCreationInfo& info)
    : vboHandler(info.staticSpriteSheetTexturePath,info.spritePixelSize),
    animationHandler(info.animatedSpriteSheetTexturePath,info.spritePixelSize)
{
    chunkHandler.setAssetSizes(
        info.tileSize,
        info.spritePixelSize,
        vboHandler.getSpriteSheetSizeInTiles().x
    );

}

void EditorHandler::update(sf::Vector2f& position)
{
	chunkHandler.update(position);

	if (chunkHandler.needsUpdate())
	{
		updateBuffers();
	}

}

void EditorHandler::render(sf::RenderTarget& window, bool shaderUniformShowVisibility)
{
    vboHandler.render(window, shaderUniformShowVisibility);
    animationHandler.render(window);
}

VBOHandler& EditorHandler::getVBOHandler()
{
    return vboHandler;
}

AnimationHandler& EditorHandler::getAnimationHandler()
{
    return animationHandler;
}

chunk::ChunkHandler& EditorHandler::getChunkHandler()
{
    return chunkHandler;
}

ChunkData* EditorHandler::getChunkData(const sf::Vector2i& position)
{
    return chunkHandler.getChunk(position);
}

sf::VertexBuffer* EditorHandler::getVBOPtr(const sf::Vector2i& position)
{
    const int chunkIndex = chunkHandler.chunkInActiveMemory(position);
    return vboHandler.getBufferPtr(chunkIndex);
}

void EditorHandler::updateBuffers()
{
    std::vector<chunk::ChunkCreationTask>& tasks = chunkHandler.getChunkCreationTasks();

    for (const chunk::ChunkCreationTask& task : tasks)
    {
       
        vboHandler.addVertexBuffer(
            task.chunkCoord,
            task.hasTileMap,
            task.chunkData,
            chunkHandler.getRenderSizes()
        );
    }

    tasks.clear();


    std::vector<uint16_t>& deletionQueue = chunkHandler.getDeletionQueue();
    for (const uint16_t& index : deletionQueue)
    {
        vboHandler.removeBuffer(index);
    }

    deletionQueue.clear();
    
    animationHandler.constrcuctAnimatedTiles(chunkHandler);

}
