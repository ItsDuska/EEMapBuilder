#include "EditorEngine.h"
#include <chrono>
#include <fstream>
#include <filesystem>
#include <cstdlib> // rand() ja srand()
#include <ctime> // time()

#include "chunk/ChunkUtils.h"

constexpr uint32_t TILEMAP_SIZE = 32u*32u;
constexpr uint32_t TILEMAP_WIDTH = 32u;


static int getRandomNumberInRange(int min, int max) {
	return min + std::rand() % (max - min + 1);
}

int EditorEngine::getAnimatedIndex(int index,AnimationCache& cache)
{
	int total = cache.getMaxSprites();
	index = (index % total + total) % total;
	return std::max(index,1);
}

BlockSelection& EditorEngine::getInventory()
{
	return gui;
}

EditorEngine::EditorEngine(sf::Vector2f& windowSize,sf::Vector2f& tileSize)
	: undoStack(20)
{
	this->windowSize = windowSize;
	this->tileSize = tileSize;
	lastPosition = {};
	spritePixelSize = { 16.f,16.f };
	currentTexCoord = {};
	currentMousePosition = {};
	lockDataEditorUpdate = false;

	lastIndex = -5;
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	info.spritePixelSize = spritePixelSize;
	info.tileSize = tileSize;
	info.staticSpriteSheetTexturePath = "data/texture/Tuukka.png";
	info.animatedSpriteSheetTexturePath = "data/texture/Olli"; // ei tarvi lopetetta t‰lle
	info.objectSpriteSheetTexturePath = "data/texture/Aku"; // ei tarvi lopetetta t‰lle
	info.layeredSpriteSheetTexturePath = "data/texture/Pietari.png";

	editorHandler = std::make_unique<EditorHandler>(info);
	spritesPerRow = editorHandler->getVBOHandler().getSpriteSheetSizeInTiles().x;

	currentTexture.setTexture(editorHandler->getVBOHandler().getTexture());
	currentTexture.setScale(sf::Vector2f(4.f, 4.f));
	sf::IntRect rect(0, 0, spritePixelSize.x,spritePixelSize.y);
	currentTexture.setTextureRect(rect);


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

	sf::Texture* texturePtrs[MAX_TAB_COUNT] =
	{
		&editorHandler->getVBOHandler().getTexture(),
		&editorHandler->getAnimationHandler().getTexture(),
		&editorHandler->getEntityHandler().getTexture(),
		&editorHandler->getLayeredTileHandler().getTexture()
	};

	VBOHandler &vbo = editorHandler->getVBOHandler();

	AnimationHandler& animationHandler = editorHandler->getAnimationHandler();

	const int staticTotalSprites = vbo.getSpriteSheetSizeInTiles().x * vbo.getSpriteSheetSizeInTiles().y;
	const int layerTotalSprites = editorHandler->getLayeredTileHandler().getSpriteSheetSizeInTiles().x * editorHandler->getLayeredTileHandler().getSpriteSheetSizeInTiles().y;
	const int entityTotalSprites = editorHandler->getEntityHandler().getSpriteSheetSizeInTiles().x * editorHandler->getEntityHandler().getSpriteSheetSizeInTiles().y;

	PackedTabInformation packedInfo[MAX_TAB_COUNT] =
	{
		{staticTotalSprites, vbo.getSpriteSheetSizeInTiles()},
		{animationHandler.getAnimationCache().getMaxSprites(), animationHandler.getSpriteSheetSizeInTiles()},
		{entityTotalSprites, editorHandler->getEntityHandler().getSpriteSheetSizeInTiles()},
		{layerTotalSprites, editorHandler->getLayeredTileHandler().getSpriteSheetSizeInTiles()}
	};

	sf::Vector2i tempSize(spritePixelSize.x, spritePixelSize.y);
	gui.awake(windowSize, tempSize, tileSize, packedInfo, font, texturePtrs);
	gui.constructElements(editorHandler->getAnimationHandler().getAnimationCache().getStartPositionsPtr(),
		editorHandler->getEntityHandler().getAnimationCache().getStartPositionsPtr());

	dataEditor.init(windowSize, font);

}

void EditorEngine::createMap(std::string& filename)
{
	if (std::filesystem::exists(filename))
	{
		this->fileName = filename;
		editorHandler->getChunkHandler().loadFromFile(filename);
	}
}

void EditorEngine::update(EventInfo& info)
{
	sf::IntRect rect(currentTexCoord.x*this->spritePixelSize.x, currentTexCoord.y* this->spritePixelSize.x,
		this->spritePixelSize.x, this->spritePixelSize.y);
	currentTexture.setTextureRect(rect);
	

	viewOffset = { info.offset.x * tileSize.x, info.offset.y * tileSize.y };

	currentMousePosition = {((info.offset.x+spritePixelSize.x) * tileSize.x),
		((info.offset.y+spritePixelSize.y) * tileSize.y) };

	if (info.showEditor)
	{
		bool isNull = false;
		if (!lockDataEditorUpdate)
		{
			lockDataEditorUpdate = true;

			lockedMousePosition = info.mousePosition;


			if (info.currentTab == 1)
			{
				isNull = dataEditor.updateAnimationTile(getAnimatedTile(info.mousePosition, info.offset));
			}
			else if (info.currentTab == 2)
			{
				isNull = dataEditor.updateEntityTile(getEntityTile(info.mousePosition, info.offset));
			}
		}

		if (isNull)
		{
			info.showEditor = false;
			lockDataEditorUpdate = false;
			return;
		}

		sf::Vector2f floatMousePosition(info.mousePosition.x, info.mousePosition.y);
		dataEditor.update(info.buttonInfo, floatMousePosition, info.currentTab);
		
		return;
	}


	if (lockDataEditorUpdate)
	{
		if (info.currentTab == 1)
		{
			auto* tile = getAnimatedTile(lockedMousePosition, info.offset);
			if (tile)
			{
				AnimationTile& newTile = dataEditor.getAnimationTile();
				std::memcpy(tile, &newTile, sizeof(AnimationTile));
			}
			
		}
		else
		{
			auto* tile = getEntityTile(lockedMousePosition, info.offset);
			if (tile)
			{
				EntityTile& newTile = dataEditor.getEntityTile();
				std::memcpy(tile, &newTile, sizeof(EntityTile));
			}
			
		}

		lockDataEditorUpdate = false;
	}

	dataEditor.updateCurrenGUIType(info.currentTab);


	editorHandler->update(currentMousePosition);

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

	int possibleFutureBlockTextureIndex = 0;
	int guiCurrentTextureIndex = 0;

	AnimationCache& animationCache = editorHandler->getAnimationHandler().getAnimationCache();
	AnimationCache& entityAnimationCache = editorHandler->getEntityHandler().getAnimationCache();
	
	switch (info.currentTab)
	{
	case 0:
		possibleFutureBlockTextureIndex = info.guiIndex;
		guiCurrentTextureIndex = possibleFutureBlockTextureIndex;

		currentTexture.setTexture(editorHandler->getVBOHandler().getTexture());
		break;
	case 1:
		currentTexture.setTexture(editorHandler->getAnimationHandler().getTexture());
		
		possibleFutureBlockTextureIndex = this->getAnimatedIndex(info.guiIndex, animationCache);

		info.guiIndex = info.guiIndex % animationCache.getMaxSprites();
		
		guiCurrentTextureIndex = animationCache.getStartPosition(possibleFutureBlockTextureIndex);
		break;
	case 2:
		currentTexture.setTexture(editorHandler->getEntityHandler().getTexture());

		possibleFutureBlockTextureIndex = this->getAnimatedIndex(info.guiIndex, entityAnimationCache);

		info.guiIndex = info.guiIndex % entityAnimationCache.getMaxSprites();

		guiCurrentTextureIndex = entityAnimationCache.getStartPosition(possibleFutureBlockTextureIndex);
		break;

	case 3:
		possibleFutureBlockTextureIndex = info.guiIndex;
		guiCurrentTextureIndex = possibleFutureBlockTextureIndex;

		currentTexture.setTexture(editorHandler->getLayeredTileHandler().getTexture());
		break;
	default:
		break;
	}

	currentTexCoord.x = guiCurrentTextureIndex % spritesPerRow;
	currentTexCoord.y = guiCurrentTextureIndex / spritesPerRow;

	if (info.activeInventory)
	{
		if (info.mode != EditMode::IDLE)
		{
			info.guiIndex = gui.select(info.mousePosition);
		}


		return;
	}

	switch (info.mode)
	{
	case EditMode::IDLE:
		break;
	case EditMode::ADD:
		switch (info.currentTab)
		{
		case 0:
			addBlock(info.mousePosition, info.offset, possibleFutureBlockTextureIndex, info.solidMode);
			break;
		case 1:
			addAnimatedBlock(info.mousePosition, info.offset, possibleFutureBlockTextureIndex, 0);
			break;
		case 2:
			addEntity(info.mousePosition, info.offset, possibleFutureBlockTextureIndex);
			break;
		case 3:
			addLayeredBlock(info.mousePosition, info.offset, possibleFutureBlockTextureIndex);
			break;
		default:
			break;
		}
				
		break;
	case EditMode::DELETE:
		switch (info.currentTab)
		{
		case 0:
			addBlock(info.mousePosition, info.offset, 0, info.solidMode);
			break;
		case 1:
			addAnimatedBlock(info.mousePosition, info.offset, 0, 0);
			break;
		case 2:
			addEntity(info.mousePosition, info.offset, 0);
			break;
		case 3:
			addLayeredBlock(info.mousePosition, info.offset, 0);
			break;
		
		default:
			break;
		}
		break;
	case EditMode::INSPECT:
		info.guiIndex = inspectBlock(info.mousePosition, info.offset);
		break;
	default:
		break;
	}

	chunk::ChunkHandler& chunkHandle = editorHandler->getChunkHandler();
	editorHandler->getAnimationHandler().UpdateVATexCoords(chunkHandle);
	editorHandler->getEntityHandler().UpdateVATexCoords(chunkHandle);
}

void EditorEngine::saveMap(std::string& filename)
{
	const auto start = std::chrono::high_resolution_clock::now();
	editorHandler->getChunkHandler().saveToFile(filename);
	const auto end = std::chrono::high_resolution_clock::now();

	const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	std::cout << "Saving duration: " << duration.count() << "milliseconds\n";
}

void EditorEngine::hardReset()
{
	undoStack.destroyBuffers();

	editorHandler.reset();

	// Tuhoo tiedosto.
	if (std::filesystem::remove(fileName))
	{
		std::cout << "guh???\n";
	}

	editorHandler = std::make_unique<EditorHandler>(info);

	sf::Texture* texturePtrs[MAX_TAB_COUNT] =
	{
		&editorHandler->getVBOHandler().getTexture(),
		&editorHandler->getAnimationHandler().getTexture(),
		&editorHandler->getEntityHandler().getTexture(),
		&editorHandler->getLayeredTileHandler().getTexture()
	};

	gui.updateTexturePtrs(texturePtrs);

	createMap(fileName);
}

void EditorEngine::drawGUI(sf::RenderTarget& window,bool enableInventoryRendering,bool enableEditorRendering)
{
	window.draw(currentSpriteHolderBox);
	window.draw(currentTexture);
	window.draw(infoText);

	if (enableInventoryRendering)
	{
		gui.draw(window);
	}

	if (enableEditorRendering)
	{
		dataEditor.render(window);
	}

}

void EditorEngine::renderMap(sf::RenderTarget& window,bool showVisibility)
{
	sf::View view = window.getDefaultView();
	sf::Vector2f newView = view.getCenter();
	newView = { newView.x + viewOffset.x, newView.y + viewOffset.y };
	view.setCenter(newView);

	window.setView(view);
	editorHandler->render(window, showVisibility);

	window.setView(window.getDefaultView());
	
}

void EditorEngine::executeUndoAction(sf::Vector2i& offset)
{
	Action action;
	if (!undoStack.undo(action))
	{
		return;
	}

	ChunkPositions positions;
	calculateChunkPositions(positions, action.mousePosition, action.offset);

	updateVBOAndMap(positions.positionInWorld,
		positions.chunkPosition,
		positions.positionInChunk,
		action.textureIndexOld,
		action.solidModeOld
	);
}

void EditorEngine::executeRedoAction(sf::Vector2i& offset)
{
	Action action;
	if (!undoStack.redo(action))
	{
		return;
	}

	ChunkPositions positions;
	calculateChunkPositions(positions, action.mousePosition, action.offset);

	updateVBOAndMap(positions.positionInWorld,
		positions.chunkPosition,
		positions.positionInChunk,
		action.textureIndexCurrent,
		action.solidModeCurrent
	);
}

void EditorEngine::resetAnimationRandomness()
{
	chunk::ChunkHandler& chunkHandle = editorHandler->getChunkHandler();
	editorHandler->getAnimationHandler().resetAnimationRandomness(chunkHandle);
}

void EditorEngine::addBlock(sf::Vector2i& position, sf::Vector2i& offset,
	const int guiIndex, bool isSolid)
{
	ChunkPositions positions{};
	if (!calculateChunkPositions(positions, position, offset))
	{
		return;
	}

	if (guiIndex == 0)
	{
		currentTexCoord.x = 0;
		currentTexCoord.y = 0;
	}

	ChunkData* chunkData = editorHandler->getChunkData(positions.chunkPosition);
	sf::VertexBuffer* buffer = editorHandler->getVBOPtr(positions.chunkPosition);

	if (chunkData == nullptr || buffer == nullptr)
	{
		return;
	}

	//GET DATA FOR UNDO
	Action action{};
	action.mousePosition = position;
	action.offset = offset;
	action.textureIndexOld = chunkData->tilemap[positions.indexToTileMap];
	action.solidModeOld = chunk::isBitSet(chunkData->solidBlockData[positions.positionInChunk.y], positions.positionInChunk.x);
	action.textureIndexCurrent = guiIndex;
	action.solidModeCurrent = isSolid;

	undoStack.addAction(action);

	updateVBOAndMap(positions.positionInWorld, positions.chunkPosition, positions.positionInChunk, guiIndex, isSolid);
}

void EditorEngine::addEntity(sf::Vector2i& position, sf::Vector2i& offset, const int guiIndex)
{
	ChunkPositions positions{};
	if (!calculateChunkPositions(positions, position, offset))
	{
		return;
	}

	//GET DATA FOR UNDO
	//Action action{};
	//action.mousePosition = position;
	//action.offset = offset;
	//action.textureIndexOld = chunkData->tilemap[index];
	//action.solidModeOld = chunk::isBitSet(chunkData->solidBlockData[positionInGrid.y], positionInGrid.x);
	//action.textureIndexCurrent = guiIndex;
	//action.solidModeCurrent = isSolid;

	if (guiIndex == 0)
	{
		currentTexCoord.x = 0;
		currentTexCoord.y = 0;
	}


	sf::Vector2<std::uint8_t> inChunk(
		static_cast<std::uint8_t>(positions.positionInChunk.x),
		static_cast<std::uint8_t>(positions.positionInChunk.y)
	);

	auto& chunkHandler = editorHandler->getChunkHandler();

	std::vector<EntityTile>& tiles = chunkHandler.getEditorSideData(positions.chunkPosition.x, positions.chunkPosition.y)->entities;


	bool updated = false;

	for (EntityTile& tile : tiles) {
		if (tile.animation.positionInChunk == inChunk)
		{
			tile.animation.textureID = guiIndex;
			updated = true;
			break;
		}
	}

	if (!updated)
	{
		EntityTile newTile;
		newTile.animation.positionInChunk = inChunk;
		newTile.animation.textureID = guiIndex;
		newTile.animation.frameDelay = 4; //getRandomNumberInRange(3,6); // Viive v‰lill‰ 1-5
		newTile.animation.currentFrame = 0;
		newTile.animation.elapsedFrames = 0;
		newTile.actionId = 0;
		newTile.type = 0;
		tiles.push_back(newTile);
	}

	editorHandler->getEntityHandler().constructTileBuffer(chunkHandler);
	lastPosition = positions.positionInWorld;
}

void EditorEngine::addAnimatedBlock(sf::Vector2i& position, sf::Vector2i& offset, const int guiIndex, bool isSolid)
{
	ChunkPositions positions{};
	if (!calculateChunkPositions(positions, position, offset))
	{
		return;
	}

	//GET DATA FOR UNDO
	//Action action{};
	//action.mousePosition = position;
	//action.offset = offset;
	//action.textureIndexOld = chunkData->tilemap[index];
	//action.solidModeOld = chunk::isBitSet(chunkData->solidBlockData[positionInGrid.y], positionInGrid.x);
	//action.textureIndexCurrent = guiIndex;
	//action.solidModeCurrent = isSolid;

	if (guiIndex == 0)
	{
		currentTexCoord.x = 0;
		currentTexCoord.y = 0;
	}


	sf::Vector2<std::uint8_t> inChunk(
		static_cast<std::uint8_t>(positions.positionInChunk.x),
		static_cast<std::uint8_t>(positions.positionInChunk.y)
	);

	auto& chunkHandler = editorHandler->getChunkHandler();

	std::vector<AnimationTile>& tiles = chunkHandler.getEditorSideData(positions.chunkPosition.x, positions.chunkPosition.y)->animations;
	

	bool updated = false;

	for (AnimationTile& tile : tiles) {
		if (tile.positionInChunk == inChunk)
		{
			tile.textureID = guiIndex;
			updated = true;
			break;
		}
	}

	if (!updated)
	{
		AnimationTile newTile;
		newTile.positionInChunk = inChunk;
		newTile.textureID = guiIndex;
		newTile.frameDelay = 4; //getRandomNumberInRange(3,6); // Viive v‰lill‰ 1-5
		newTile.currentFrame = 0;
		newTile.elapsedFrames = 0;
		tiles.push_back(newTile);
	}

	editorHandler->getAnimationHandler().constructTileBuffer(chunkHandler);
	lastPosition = positions.positionInWorld;
}


AnimationTile* EditorEngine::getAnimatedTile(sf::Vector2i& position, sf::Vector2i& offset)
{
	ChunkPositions positions{};
	calculateChunkPositions(positions, position, offset,true);
	
	sf::Vector2<std::uint8_t> inChunk(
		static_cast<std::uint8_t>(positions.positionInChunk.x),
		static_cast<std::uint8_t>(positions.positionInChunk.y)
	);

	auto& chunkHandler = editorHandler->getChunkHandler();

	std::vector<AnimationTile>& tiles = chunkHandler.getEditorSideData(positions.chunkPosition.x, positions.chunkPosition.y)->animations;

	for (AnimationTile& tile : tiles) {
		if (tile.positionInChunk == inChunk)
		{
			return &tile;
		}
	}

	// guh t‰‰ muute voi crashaa kai :D
	// Ainaki sit huomaa et jotain hassua k‰vi
	//std::cerr << "ERROR: Didn't find animation tile!\n";
	return nullptr;
}

EntityTile* EditorEngine::getEntityTile(sf::Vector2i& position, sf::Vector2i& offset)
{
	ChunkPositions positions{};
	calculateChunkPositions(positions, position, offset,true);

	sf::Vector2<std::uint8_t> inChunk(
		static_cast<std::uint8_t>(positions.positionInChunk.x),
		static_cast<std::uint8_t>(positions.positionInChunk.y)
	);

	auto& chunkHandler = editorHandler->getChunkHandler();

	std::vector<EntityTile>& tiles = chunkHandler.getEditorSideData(positions.chunkPosition.x, positions.chunkPosition.y)->entities;

	for (EntityTile& tile : tiles) {
		if (tile.animation.positionInChunk == inChunk)
		{
			return &tile;
		}
	}

	// guh t‰‰ muute voi crashaa kai :D
	// Ainaki sit huomaa et jotain hassua k‰vi
	std::cerr << "ERROR: Didn't find animation tile!\n";
	return nullptr;
}


void EditorEngine::addLayeredBlock(sf::Vector2i& position, sf::Vector2i& offset, const int guiIndex)
{
	ChunkPositions positions{};
	if (!calculateChunkPositions(positions, position, offset))
	{
		return;
	}

	if (guiIndex == 0)
	{
		currentTexCoord.x = 0;
		currentTexCoord.y = 0;
	}

	sf::Vector2<std::uint8_t> inChunk(
		static_cast<std::uint8_t>(positions.positionInChunk.x),
		static_cast<std::uint8_t>(positions.positionInChunk.y)
	);

	auto& chunkHandler = editorHandler->getChunkHandler();

	std::vector<LayeredStaticTile>& tiles = chunkHandler.getEditorSideData(positions.chunkPosition.x, positions.chunkPosition.y)->layeredTiles;


	bool updated = false;

	for (LayeredStaticTile& tile : tiles) {
		if (tile.positionInChunk == inChunk)
		{
			tile.textureID = guiIndex;
			updated = true;
			break;
		}
	}

	if (!updated)
	{
		LayeredStaticTile newTile;
		newTile.positionInChunk = inChunk;
		newTile.textureID = guiIndex;
		tiles.push_back(newTile);
	}

	editorHandler->getLayeredTileHandler().constractBuffer(chunkHandler);
	lastPosition = positions.positionInWorld;
}

int EditorEngine::inspectBlock(sf::Vector2i& position, sf::Vector2i& offset)
{
	ChunkPositions positions{};
	calculateChunkPositions(positions, position, offset);

	ChunkData* chunkData = editorHandler->getChunkData(positions.chunkPosition);

	if (chunkData == nullptr)
	{
		return 0;
	}

	return static_cast<int>(chunkData->tilemap[positions.indexToTileMap]);
}

bool EditorEngine::calculateChunkPositions(ChunkPositions& positions, sf::Vector2i& mousePosition, sf::Vector2i& offset,bool disableLastPositionCheck)
{
	sf::Vector2i newPosition(mousePosition.x / tileSize.x, mousePosition.y / tileSize.y);
	newPosition += offset;

	if (!disableLastPositionCheck && newPosition == lastPosition)
	{
		return false;
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

	positions.positionInWorld = newPosition;
	positions.chunkPosition = chunkPosition;
	positions.positionInChunk = positionInGrid;
	positions.indexToTileMap = positionInGrid.y * TILEMAP_WIDTH + positionInGrid.x;
	return true;
}

void EditorEngine::updateVBOAndMap(const sf::Vector2i& vertPosition, const sf::Vector2i& chunkPosition, const sf::Vector2i& positionInChunk, const int textureIndex, const bool solidMode)
{
	const int index = positionInChunk.y * TILEMAP_WIDTH + positionInChunk.x;

	ChunkData* chunkData = editorHandler->getChunkData(chunkPosition);

	sf::VertexBuffer* buffer = editorHandler->getVBOPtr(chunkPosition);

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
	chunk::addQuadVertices(quad,vertPosition, currentTexCoord, tileSize, spritePixelSize, solidMode);
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
		"\nEdit Tile (Q)"
		"\nMove (WASD) duh"
		"\nGet Block (MMB)"
		"\nUndo (Z)"
		"\nRedo (X)"
		"\nTabs (1-4)"
		"\nReset animation Randomness (G)"
		"\nHard Reset (DELETE)"; 


	infoText.setString(rawText);
}

