#include "EditorEngine.h"
#include <chrono>
#include <fstream>
#include <filesystem>


constexpr uint32_t TILEMAP_SIZE = 32u*32u;
constexpr uint32_t TILEMAP_WIDTH = 32u;

EditorEngine::EditorEngine(sf::Vector2f& windowSize,sf::Vector2f& tileSize)
	: undoStack(20)
{
	this->windowSize = windowSize;
	this->tileSize = tileSize;
	lastPosition = {};
	textureSize = { 16.f,16.f };
	currentTexCoord = {};
	currentMousePosition = {};
	sheetWidthInTiles = 0;

	handler = std::make_unique<chunk::ChunkHandler>();

	std::string spriteSheet = "data/texture/Tuukka.png";
	if (!texture.loadFromFile(spriteSheet))
	{
		std::cerr << "ERROR: Can't open texture!\n";
		return;
	}
	states.texture = &texture;

	currentTexture.setTexture(texture);
	currentTexture.setScale(sf::Vector2f(4.f, 4.f));
	sf::IntRect rect(0, 0, textureSize.x,textureSize.y);
	currentTexture.setTextureRect(rect);

	spriteSheetSize = texture.getSize();
	sheetWidthInTiles = spriteSheetSize.x / textureSize.x;

	handler->setAssetSizes(tileSize, textureSize,sheetWidthInTiles);

	spritesPerRow = spriteSheetSize.x / textureSize.x;
	spritesPerColumn = spriteSheetSize.y / textureSize.y;
	totalSprites = spritesPerRow * spritesPerColumn;

	if (!shader.loadFromFile("src/shaders/shader.vert", "src/shaders/shader.frag"))
	{
		std::cerr << "ERROR: Can't load shaders!\n";
	}

	states.shader = &shader;


	if (!font.loadFromFile("data/fonts/DotGothic16-Regular.ttf"))
	{
		std::cerr << "ERROR: Failed to load font... guh?\n";
	}

	infoText.setFont(font);

	infoText.setPosition(4.f, 0.f);
	clock.restart();

	EventInfo nullInfo{};
	updateTextDisplay(nullInfo);

	
	currentTexture.setPosition(4.f, infoText.getGlobalBounds().height*1.2f);
	currentSpriteHolderBox.setPosition(currentTexture.getPosition().x-5,currentTexture.getPosition().y - currentTexture.getGlobalBounds().height / 10);
	currentSpriteHolderBox.setOrigin(currentTexture.getOrigin());
	
	currentSpriteHolderBox.setSize(currentTexture.getGlobalBounds().getSize());
	currentSpriteHolderBox.setScale(1.2f, 1.2f);
	currentSpriteHolderBox.setFillColor(sf::Color(0,0,0,50));

}

void EditorEngine::createMap(std::string& filename)
{
	if (std::filesystem::exists(filename))
	{
		this->fileName = filename;
		handler->loadFromFile(filename);
	}
}


void EditorEngine::update(EventInfo& info)
{
	if (info.guiIndex <= 1)
	{
		info.guiIndex = (info.guiIndex % totalSprites + totalSprites) % totalSprites;
	}
	else
	{
		info.guiIndex = (info.guiIndex % totalSprites);
	}

	currentTexCoord.x = info.guiIndex % spritesPerRow;
	currentTexCoord.y = info.guiIndex / spritesPerRow;

	sf::IntRect rect(currentTexCoord.x*this->textureSize.x, currentTexCoord.y* this->textureSize.x,
		this->textureSize.x, this->textureSize.y);
	currentTexture.setTextureRect(rect);
	

	viewOffset = { info.offset.x * tileSize.x, info.offset.y * tileSize.y };

	currentMousePosition = {((info.offset.x+textureSize.x) * tileSize.x),
		((info.offset.y+textureSize.y) * tileSize.y) };

	handler->update(currentMousePosition);

	shader.setUniform("solidBlockVisibility", static_cast<float>(info.showSolidBlocks));

	if (info.hardReset)
	{
		return;
	}

	updateTextDisplay(info);

	const sf::Int32 cooldown = 35;
	if (clock.getElapsedTime().asMilliseconds() <= cooldown)
	{
		return;
	}
	clock.restart();

	switch (info.mode)
	{
	case EditMode::IDLE:
		return;
	case EditMode::ADD:
		addBlock(info.mousePosition, info.offset,info.guiIndex,info.solidMode); 
		break;
	case EditMode::DELETE:
		addBlock(info.mousePosition, info.offset, 0, info.solidMode); 
		break;
	case EditMode::INSPECT:
		info.guiIndex = inspectBlock(info.mousePosition, info.offset);
		break;
	default:
		break;
	}


}

void EditorEngine::saveMap(std::string& filename)
{
	const auto start = std::chrono::high_resolution_clock::now();
	handler->saveToFile(filename);
	const auto end = std::chrono::high_resolution_clock::now();

	const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	std::cout << "Saving duration: " << duration.count() << "milliseconds\n";
}

void EditorEngine::hardReset()
{
	handler.reset();

	// Tuhoo tiedosto.
	if (std::filesystem::remove(fileName))
	{
		std::cout << "guh???\n";
	}
	handler = std::make_unique<chunk::ChunkHandler>();
	handler->setAssetSizes(tileSize, textureSize, sheetWidthInTiles);
	createMap(fileName);
}

void EditorEngine::drawGUI(sf::RenderTarget& window)
{
	window.draw(currentSpriteHolderBox);
	window.draw(currentTexture);
	window.draw(infoText);
}

void EditorEngine::renderMap(sf::RenderTarget& window)
{
	sf::View view = window.getDefaultView();
	sf::Vector2f newView = view.getCenter();
	newView = { newView.x + viewOffset.x, newView.y + viewOffset.y };
	view.setCenter(newView);

	window.setView(view);
	handler->renderActiveChunks(window, states);
	window.setView(window.getDefaultView());
	
}

void EditorEngine::executeUndoAction()
{
	Action action;
	undoStack.undo(action);

	updateVBOAndMap(action.vertexPosition,
		action.chunkPosition,
		action.positionInChunk,
		action.textureIndexOld,
		action.solidModeOld
	);
}

void EditorEngine::executeRedoAction()
{
	Action action;
	undoStack.redo(action);

	updateVBOAndMap(action.vertexPosition,
		action.chunkPosition,
		action.positionInChunk,
		action.textureIndexCurrent,
		action.solidModeCurrent
	);

}

void EditorEngine::addBlock(sf::Vector2i& position, sf::Vector2i& offset, const int guiIndex, bool isSolid)
{
	sf::Vector2i newPosition(position.x / tileSize.x, position.y / tileSize.y);
	newPosition += offset;
	
	if (newPosition == lastPosition)
	{
		return;
	}

	if (guiIndex == 0)
	{
		currentTexCoord.x = 0;
		currentTexCoord.y = 0;
	}

	const int width = static_cast<int>(TILEMAP_WIDTH);

	const sf::Vector2i chunkPosition(
		newPosition.x >= 0 ? newPosition.x / width : (newPosition.x - (width - 1)) / width,
		newPosition.y >= 0 ? newPosition.y / width : (newPosition.y - (width - 1)) / width
	);

	const sf::Vector2i positionInGrid(
		newPosition.x >= 0 ? newPosition.x % width : (width - 1) + ((newPosition.x + 1) % width),
		newPosition.y >= 0 ? newPosition.y % width : (width - 1) + ((newPosition.y + 1) % width)
	);

	const int index = positionInGrid.y * TILEMAP_WIDTH + positionInGrid.x;

	ChunkData* chunkData = handler->getChunk(chunkPosition);
	sf::VertexBuffer* buffer = handler->getBuffer(chunkPosition);


	if (chunkData == nullptr || buffer == nullptr)
	{
		return;
	}

	//GET DATA FOR UNDO
	Action action{};
	action.vertexPosition = newPosition;
	action.chunkPosition = chunkPosition;
	action.positionInChunk = positionInGrid;
	action.textureIndexOld = chunkData->tilemap[index];
	action.solidModeOld = chunk::isBitSet(chunkData->solidBlockData[positionInGrid.y], positionInGrid.x);
	action.textureIndexCurrent = guiIndex;
	action.solidModeCurrent = isSolid;

	undoStack.addAction(action);

	updateVBOAndMap(newPosition, chunkPosition, positionInGrid, guiIndex, isSolid);
}

int EditorEngine::inspectBlock(sf::Vector2i& position, sf::Vector2i& offset)
{
	sf::Vector2i newPosition(position.x / tileSize.x, position.y / tileSize.y);
	newPosition += offset;

	const int width = static_cast<int>(TILEMAP_WIDTH);

	const sf::Vector2i chunkPosition(
		newPosition.x >= 0 ? newPosition.x / width : (newPosition.x - (width - 1)) / width,
		newPosition.y >= 0 ? newPosition.y / width : (newPosition.y - (width - 1)) / width
	);

	const sf::Vector2i positionInGrid(
		newPosition.x >= 0 ? newPosition.x % width : (width - 1) + ((newPosition.x + 1) % width),
		newPosition.y >= 0 ? newPosition.y % width : (width - 1) + ((newPosition.y + 1) % width)
	);

	const int index = positionInGrid.y * TILEMAP_WIDTH + positionInGrid.x;
	ChunkData* chunkData = handler->getChunk(chunkPosition);


	if (chunkData == nullptr)
	{
		return 0;
	}

	return static_cast<int>(chunkData->tilemap[index]);
}

void EditorEngine::updateVBOAndMap(const sf::Vector2i& vertPosition, const sf::Vector2i& chunkPosition, const sf::Vector2i& positionInChunk, const int textureIndex, const bool solidMode)
{
	const int index = positionInChunk.y * TILEMAP_WIDTH + positionInChunk.x;

	ChunkData* chunkData = handler->getChunk(chunkPosition);
	sf::VertexBuffer* buffer = handler->getBuffer(chunkPosition);

	if (chunkData == nullptr || buffer == nullptr)
	{
		return;
	}

	chunkData->tilemap[index] = textureIndex;

	if (solidMode)
	{
		chunk::setBit(chunkData->solidBlockData[positionInChunk.y], positionInChunk.x);
	}
	else
	{
		chunk::clearBit(chunkData->solidBlockData[positionInChunk.y], positionInChunk.x);
	}

	currentTexCoord.x = textureIndex % spritesPerRow;
	currentTexCoord.y = textureIndex / spritesPerRow;


	sf::Vertex quad[4];
	chunk::addQuadVertices(quad,vertPosition, currentTexCoord, tileSize, textureSize, solidMode);
	size_t vertexOffset = static_cast<size_t>(index) * 4;
	buffer->update(quad, 4, vertexOffset);

	lastPosition = vertPosition;
}

void EditorEngine::updateTextDisplay(EventInfo& info)
{
	rawText = "Solid Mode (CTRL) : " + std::to_string(info.solidMode) +
		"\nShow Solid Mode (SHIFT) : " + std::to_string(info.showSolidBlocks) + 
		"\nShow Grid Mode (SPACE) : " + std::to_string(info.showLines) +
		"\nSave (TAB)"
		"\nQuit (ESC)"
		"\nOpen Menu (E)"
		"\nMove (WASD) duh"
		"\nGet Block (MMB)"
		"\nUndo (Z)"
		"\nRedo (X)"
		"\nHard Reset (DELETE)"; 


	infoText.setString(rawText);
}

