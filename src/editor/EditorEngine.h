#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <string>

#include "chunk/ChunkHandler.h"
#include "editor/gui/BlockSelection.h"


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
};


class EditorEngine
{
public:
	EditorEngine(sf::Vector2f& windowSize, sf::Vector2f& tileSize);
	void createMap(std::string& filename);
	void update(EventInfo& info);
	void saveMap(std::string& filename);

	void render(sf::RenderTarget& window);
private:
	void addBlock(sf::Vector2i& position, sf::Vector2i& offset, const int guiIndex,bool isSolid);
	void addEntity(); //TODO:
	void deleteEntity(); //TODO:

	void updateTextDisplay(EventInfo& info);

private:
	chunk::ChunkHandler handler;



	sf::Vector2f windowSize;
	sf::RenderStates states;
	sf::Texture texture;

	sf::Vector2f tileSize;

	sf::Vector2i lastPosition;

	sf::Sprite currentTexture;

	sf::Vector2f textureSize;
	sf::Vector2u spriteSheetSize;

	sf::Vector2i currentTexCoord;
	sf::Vector2f currentMousePosition;

	sf::Vector2f viewOffset;
	int sheetWidthInTiles;

	int spritesPerRow;
	int spritesPerColumn;
	int totalSprites;

	sf::Shader shader;

	sf::Font font;
	sf::Text infoText;
	std::string rawText;

	sf::Clock clock;

};

