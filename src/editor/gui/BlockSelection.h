#pragma once
#include <SFML/Graphics.hpp>

constexpr int MAX_TAB_COUNT = 4;

struct GUIBufferData
{
	sf::VertexBuffer buffer;
	int maxSpriteCount;
	sf::Vector2i sizeInTiles;
	sf::Vector2i sizeInTilesWithOffset;
	sf::Texture* texturePtr;
};

struct PackedTabInformation
{
	int maxSpriteCount;
	sf::Vector2i sizeInTiles;
};

class BlockSelection
{
public:
	void awake(sf::Vector2f& windowSize,
		sf::Vector2i& blockSize,
		sf::Vector2f& tileSize,
		PackedTabInformation* packedTabs,
		sf::Font& font,sf::Texture** textures);

	void draw(sf::RenderTarget& target);
	void updateScrollOffset(int& offset);
	void changeTab(int tab);

	void updateTexturePtrs(sf::Texture** textures);

	// returns the blocks texture index
	int select(sf::Vector2i& mousePosition); 

	void constructElements(const std::vector<sf::Vector2i>* animationStartIndices, const std::vector<sf::Vector2i>* objectStartIndices);
	void constructGUIText(sf::Font& font);
private:
	void setupBackground(sf::Vector2f windowSize);
	void constructVBO(GUIBufferData& data, const std::vector<sf::Vector2i>* animationStartIndices); // rakentaa VBO:n jossa on kaikki blockit.

	GUIBufferData& getCurrentElementData();

private:
	struct BufferView
	{
		size_t offset;
		size_t count;
	};

	sf::Vector2i blockSize; //texture size ect. 16x16
	sf::Vector2f tileSize;
	sf::Vector2f backgroundOffsetPosition;
	int currentTab;
	int currentOffset;
	float spacing;
	sf::FloatRect adjustedBounds;

	// GUI ELEMENTS
	sf::RectangleShape background;
	sf::VertexBuffer staticBlockBuffer;
	sf::RenderStates globalGUIrenderStates;
	sf::Text displayTabText[MAX_TAB_COUNT];
	GUIBufferData guiElements[MAX_TAB_COUNT];
	BufferView view;
};

/*
* 
* Mit‰ sis‰lt‰‰:
* 
* tabit. eka on palikoille ja toka on animoiduille palikoille
* klikkaa mist‰ valitaan.
*/


/* 
* Luuseri joudut tekeen t‰n oikeesti loppuun.
* https://tenor.com/view/byuntear-cat-mocking-gif-13935821400850516732
*/