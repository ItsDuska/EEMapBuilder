#include "EditorEngine.h"
#include <chrono>
#include <fstream>
#include <filesystem>

constexpr uint32_t TILEMAP_SIZE = 32u*32u;
constexpr uint32_t TILEMAP_WIDTH = 32u;


EditorEngine::EditorEngine(sf::Vector2f& windowSize,sf::Vector2f& tileSize)
{
	this->windowSize = windowSize;
	this->tileSize = tileSize;
	lastPosition = {};
	textureSize = { 16.f,16.f };
	currentTexCoord = {};
	currentMousePosition = {};
	sheetWidthInTiles = 0;

	if (!texture.loadFromFile("data/texture/TestSpriteSheet.png"))
	{
		std::cerr << "ERROR: Can't open texture!\n";
		return;
	}
	states.texture = &texture;

	currentTexture.setTexture(texture);
	currentTexture.setScale(sf::Vector2f(4.f, 4.f));
	sf::IntRect rect(0, 0, 16, 16);
	currentTexture.setTextureRect(rect);

	spriteSheetSize = texture.getSize();
	sheetWidthInTiles = spriteSheetSize.x / textureSize.x;

	handler.setAssetSizes(tileSize, textureSize,sheetWidthInTiles);

	spritesPerRow = spriteSheetSize.x / textureSize.x;
	spritesPerColumn = spriteSheetSize.y / textureSize.y;
	totalSprites = spritesPerRow * spritesPerColumn;

	if (!shader.loadFromFile("src/shaders/shader.vert", "src/shaders/shader.frag"))
	{
		std::cerr << "ERROR: Can't load shaders!\n";
	}

	states.shader = &shader;
}

void EditorEngine::createMap(std::string& filename)
{
	if (std::filesystem::exists(filename))
	{
		handler.loadFromFile(filename);
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

	currentMousePosition = {((info.offset.x+16) * tileSize.x),
		((info.offset.y+16) * tileSize.y) };

	handler.update(currentMousePosition);

	shader.setUniform("solidBlockVisibility", static_cast<float>(info.showSolidBlocks));

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
	default:
		break;
	}

	
}

void EditorEngine::saveMap(std::string& filename)
{
	const auto start = std::chrono::high_resolution_clock::now();
	handler.saveToFile(filename);
	const auto end = std::chrono::high_resolution_clock::now();

	const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	std::cout << "Saving duration: " << duration.count() << "milliseconds\n";
}

void EditorEngine::render(sf::RenderTarget& window)
{
	sf::View view = window.getDefaultView();
	sf::Vector2f newView = view.getCenter();
	newView = { newView.x + viewOffset.x, newView.y + viewOffset.y };
	view.setCenter(newView);

	window.setView(view);
	handler.renderActiveChunks(window, states);
	window.setView(window.getDefaultView());
	window.draw(currentTexture);
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

	ChunkData* chunkData = handler.getChunk(chunkPosition);
	sf::VertexBuffer* buffer = handler.getBuffer(chunkPosition);

	chunkData->tilemap[index] = guiIndex;

	if (isSolid)
	{
		chunk::setBit(chunkData->solidBlockData[positionInGrid.y], positionInGrid.x);
	}
	else
	{
		chunk::clearBit(chunkData->solidBlockData[positionInGrid.y], positionInGrid.x);
	}

	sf::Vertex quad[4];
	chunk::addQuadVertices(quad, newPosition, currentTexCoord, tileSize, textureSize,isSolid);
	size_t vertexOffset = static_cast<size_t>(index) * 4;
	buffer->update(quad, 4, vertexOffset);

	lastPosition = newPosition;	
}

