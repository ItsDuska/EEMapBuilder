#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <string>

#include "world/WorldHandler.h"

#include "chunk/ChunkHandler.h"
#include "editor/gui/BlockSelection.h"
#include "UndoStack.h"




/*
* Mahdollinen file format:
* Tiedoston ensimm‰iset 4 byte‰ kertoo staattisen listan koon.
* Seuraava 4 byte‰ kertoo muiden ei staattisten objektien listan koon. 
* T‰h‰n tallennetaan seuraavat.
* 
* // objektin / spriten sijainti staattisen listan suhteen.
* vec2i position; 
* 
* // id kyseisen objektin spritelle
* uint16_t BlockID 
* 
* // mahdolliset framet ja kuinka monta niit‰ on.
* uint16_t frameCount;
* 
* //mahdollisesti id, joka osottaa johonkin listaan, jossa on esim kylttien tekstit, tai funktio...
* // jonkonkin actioniin kuten oven avaamiseen.
* uint16_t actionID;
* 
* 
* Staattisen listan formaatti:
* blockID = id to current texture index in an int array in the program code.
* solid = boolean value to see if the block has collisions;
* 
* 
* Ennen staattista lsitaa on solidBlockData buffer data, jossa jokainen bitti kertoo onko palikka solid vai ei.
* 
* Tiedosto n‰ytt‰‰ about t‰lt‰:
* 
* 0 0 0 2 0 2 0 0 1... // solidBlockData buffer.
* 
* 0 0 0 1 1 1 1 1 2 2 1 1 .. Texture buffer...
* 
*/


enum class EditMode : char
{
	IDLE,
	ADD,
	INSPECT,
	DELETE
};

struct EventInfo
{
	EditMode mode;
	sf::Vector2i mousePosition;
	sf::Vector2i offset;
	int guiIndex;
	bool mouseActive;
	bool solidMode;
	bool showSolidBlocks;
	bool showLines;
	bool activeInventory;
	bool hardReset;
	int currentTab;

};

struct ChunkPositions
{
	sf::Vector2i positionInWorld;
	sf::Vector2i chunkPosition;
	sf::Vector2i positionInChunk;
};



/*

Siirr‰ palikan laittaminen ja inspectaaminen sek‰ undo homma omaan classiin.
ehk‰‰?

*/


class EditorEngine
{
public:
	EditorEngine(sf::Vector2f& windowSize, sf::Vector2f& tileSize);
	void createMap(std::string& filename);
	void update(EventInfo& info);
	void saveMap(std::string& filename);
	void hardReset();

	void drawGUI(sf::RenderTarget& window, bool enableInventoryRendering);
	void renderMap(sf::RenderTarget& window, bool showVisibility);

	void executeUndoAction(sf::Vector2i& offset);
	void executeRedoAction(sf::Vector2i& offset);

	void resetAnimationRandomness();
	
	BlockSelection& getInventory();

private:
	void addBlock(sf::Vector2i& position, sf::Vector2i& offset,
		const int guiIndex, bool isSolid);
	//void addEntity(); //TODO:
	//void deleteEntity(); //TODO:
	void addAnimatedBlock(sf::Vector2i& position, sf::Vector2i& offset,
		const int guiIndex, bool isSolid);

	int inspectBlock(sf::Vector2i& position, sf::Vector2i& offset);


	void calculateChunkPositions(ChunkPositions& positions, sf::Vector2i& mousePosition, sf::Vector2i& offset);

	void updateVBOAndMap(const sf::Vector2i& vertPosition,
		const sf::Vector2i& chunkPosition,
		const sf::Vector2i& positionInChunk,
		const int textureIndex, const bool solidMode);

	void updateTextDisplay(EventInfo& info);

	int getAnimatedIndex(int index, AnimationCache& cache);


private:
	std::unique_ptr <EditorHandler> editorHandler;

	sf::Vector2f windowSize;
	sf::Texture layeredObjectsTexture;

	sf::Vector2f tileSize;
	sf::Vector2i lastPosition;
	sf::Sprite currentTexture;
	sf::Vector2f spritePixelSize;

	sf::Vector2i currentTexCoord;
	sf::Vector2f currentMousePosition;

	sf::Vector2f viewOffset;

	int spritesPerRow;
	int spritesPerColumn;
	int totalSprites;

	sf::Font font;
	sf::Text infoText;
	std::string rawText;

	sf::Clock clock;
	std::string fileName;

	sf::RectangleShape currentSpriteHolderBox;
	UndoStack undoStack;
	int lastIndex;

	//Inventory
	BlockSelection gui;
	EditorCreationInfo info;

};

