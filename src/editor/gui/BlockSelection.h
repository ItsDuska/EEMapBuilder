#pragma once
#include <SFML/Graphics.hpp>

class BlockSelection
{
public:
	BlockSelection(sf::Vector2f& windowSize, sf::Vector2i& blockSize,sf::Vector2f& staticImageSize, sf::Vector2f& animatedImageSize);


	void draw(sf::RenderTarget& target, sf::Texture& spriteSheetStatic, sf::Texture& spriteSheetAnimated) const;


	// returns the blocks texture index
	int select(sf::Vector2f& mousePosition); 

private:
	void constructElements(); // rakentaa VBO:n jossa on kaikki blockit.


private:
	sf::Vector2i blockSize;
	sf::Vector2i staticSize;
	sf::Vector2i animatedSize;
	

	// GUI ELEMENTS
	sf::RectangleShape background;

	sf::VertexBuffer staticBlockBuffer;

	sf::VertexArray animatedBlockBuffer;

};

/*
* 
* Mit‰ sis‰lt‰‰:
* 
* tabit. eka on palikoille ja toka on animoiduille palikoille
* klikkaa mist‰ valitaan.
* 
* 
* Looseri joudut tekeen t‰n oikeesti loppuun.
* https://tenor.com/view/byuntear-cat-mocking-gif-13935821400850516732
*/