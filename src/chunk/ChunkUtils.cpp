#include "ChunkUtils.h"



void chunk::addQuadVertices(sf::Vertex* quad, const sf::Vector2i& position, const sf::Vector2i& texCoord, const sf::Vector2f& tileSize, const sf::Vector2f& textureSize, const bool isSolid)
{
	quad[0].position = sf::Vector2f(position.x * tileSize.x, position.y * tileSize.y);
	quad[1].position = sf::Vector2f((position.x + 1) * tileSize.x, position.y * tileSize.y);
	quad[2].position = sf::Vector2f((position.x + 1) * tileSize.x, (position.y + 1) * tileSize.y);
	quad[3].position = sf::Vector2f(position.x * tileSize.x, (position.y + 1) * tileSize.y);

	quad[0].texCoords = sf::Vector2f(texCoord.x * textureSize.x, texCoord.y * textureSize.y);
	quad[1].texCoords = sf::Vector2f((texCoord.x + 1) * textureSize.x, texCoord.y * textureSize.y);
	quad[2].texCoords = sf::Vector2f((texCoord.x + 1) * textureSize.x, (texCoord.y + 1) * textureSize.y);
	quad[3].texCoords = sf::Vector2f(texCoord.x * textureSize.x, (texCoord.y + 1) * textureSize.y);

	if (isSolid)
	{
		return;
	}

	sf::Color color = sf::Color::Black;
	quad[0].color = color;
	quad[1].color = color;
	quad[2].color = color;
	quad[3].color = color;
}

bool chunk::isBitSet(uint32_t number, int n)
{
	return (number & (1 << n)) != 0;
}

void chunk::setBit(uint32_t& number, int n)
{
	number |= (1 << n);
}

void chunk::clearBit(uint32_t& number, int n)
{
	number &= ~(1 << n);
}
