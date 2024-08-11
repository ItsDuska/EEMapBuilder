#pragma once
#include <SFML/Graphics.hpp>


constexpr int MAX_TAB_COUNT = 4;




class BlockSelection
{
public:
	//BlockSelection(sf::Vector2f& windowSize, sf::Vector2i& blockSize,sf::Vector2f& staticImageSize, sf::Vector2f& animatedImageSize);



	void awake(sf::Vector2f& windowSize,
		sf::Vector2i& blockSize,
		int maxStaticTextures,
		int maxAniamatedTextures,
		sf::Vector2f& tileSize,
		int staticTextureWidth,
		int animatedTextuereWidth,
		int staticTextureHeight,
		int animatedTextuereHeight,
		sf::Font& font);

	void draw(sf::RenderTarget& target, sf::Texture& spriteSheetStatic, sf::Texture* spriteSheetAnimated);
	void updateScrollOffset(int& offset);

	// returns the blocks texture index
	int select(sf::Vector2i& mousePosition); 


	void constructElements(); // rakentaa VBO:n jossa on kaikki blockit.
	void constructGUIText(sf::Font& font);
private:
	void setupBackground(sf::Vector2f windowSize);



private:
	struct BufferView
	{
		size_t offset;
		size_t count;
	};




	sf::Vector2i blockSize; //texture size ect. 16x16
	sf::Vector2f tileSize;
	sf::Vector2i staticSize; //??
	sf::Vector2i animatedSize; // ??
	sf::Vector2f baseWindowSize; //GUi ikkunan koko
	sf::Vector2i staticBlockSizeInTiles;

	int maxStaticTextures;
	int maxAniamatedTextures;
	
	int staticTextureWidthInTiles;
	int animatedTextuereWidthInTiles;
	int staticTextureHeightInTiles;
	int animatedTextuereHeightInTiles;

	// GUI ELEMENTS
	sf::RectangleShape background;

	sf::VertexBuffer staticBlockBuffer;

	sf::VertexArray animatedBlockBuffer;

	sf::RenderStates staticBufferStates;

	sf::Text displayTabText[MAX_TAB_COUNT];


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