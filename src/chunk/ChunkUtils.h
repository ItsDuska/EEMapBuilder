#pragma once
#include <cstdint>
#include <SFML/Graphics/Vertex.hpp>

namespace chunk
{
	void addQuadVertices(sf::Vertex* quad,
		const sf::Vector2i& position,
		const sf::Vector2i& texCoord,
		const sf::Vector2f& tileSize,
		const sf::Vector2f& textureSize,
		const bool isSolid
	);

	bool isBitSet(uint32_t number, int n);

	void setBit(uint32_t& number, int n);

	void clearBit(uint32_t& number, int n);
}
