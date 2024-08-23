#pragma once
#include "animation/BaseAnimationHandler.h"

class EntityHandler : public BaseAnimationHandler
{
public:
	EntityHandler(std::string& filepath, sf::Vector2f& spritePixelSize);

	void constructTileBuffer(chunk::ChunkHandler& handler);
	void UpdateVATexCoords(chunk::ChunkHandler& handler);

	// wah
private:

	// Guh
};

