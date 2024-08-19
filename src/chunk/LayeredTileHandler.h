#pragma once
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Texture.hpp>
//#include <string>

namespace chunk
{
	class ChunkHandler;
}


class LayeredTileHandler
{
public:
	LayeredTileHandler(std::string& animationFileName, sf::Vector2f& spritePixelSize);

	void render(sf::RenderTarget& window);

	void constractBuffer(chunk::ChunkHandler& handler);

	sf::Texture& getTexture();
	sf::Vector2i& getSpriteSheetSizeInTiles();
private:
	sf::VertexArray vaTiles;
	sf::Texture texture;
	sf::Vector2i spriteSheetSizeInTiles;

};

