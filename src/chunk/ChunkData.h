#pragma once
#include <stdint.h>
#include <SFML/System/Vector2.hpp>

constexpr auto CHUNK_SIZE = 32;
constexpr auto TILE_MAP_SIZE = CHUNK_SIZE * CHUNK_SIZE;


struct BufferView
{
	uint16_t offset; // Indeksi, josta interaktiiviset elementit alkavat
	uint16_t count; // Interaktiivisten elementtien m‰‰r‰
};

struct ChunkData
{
	int16_t x;
	int16_t y;
	uint16_t tilemap[TILE_MAP_SIZE]; 
	uint32_t solidBlockData[CHUNK_SIZE]; 
	BufferView interactiveBuffer;
	BufferView animatedObjectBuffer;
};

struct InteractiveTile {
	sf::Vector2<uint8_t> tilePosition;
	uint16_t type; // Esimerkkin‰ ovi, joka vie toiseen huoneeseen. 1 voi olla ovi, 2 kyltti ja jne...
	uint16_t actionId; //Esim huoneen id johon ovi voi vide‰ tai vaikkapa kyltin tekstin indeksi listaan, joka on p‰in vattua.
};

/*
* Luo jokaiselle chunkille vertexArray.
* T‰ss‰ vertex arrayssa on kaikki animoidut asiat kuten ruoho ja semmonen
*/

struct AnimatedObjects
{
	sf::Vector2<uint8_t> tilePosition;
	uint16_t textureID;
	uint8_t frameCount;
};
