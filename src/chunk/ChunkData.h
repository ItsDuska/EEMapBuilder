#pragma once
#include <stdint.h>
#include <SFML/System/Vector2.hpp>

constexpr auto CHUNK_SIZE = 32;
constexpr auto TILE_MAP_SIZE = CHUNK_SIZE * CHUNK_SIZE;


struct BufferView
{
	uint16_t offset; // Indeksi, josta interaktiiviset elementit alkavat
	uint16_t count; // Interaktiivisten elementtien m��r�
};

struct ChunkData
{
	int16_t x;
	int16_t y;
	uint16_t tilemap[TILE_MAP_SIZE]; 
	uint32_t solidBlockData[CHUNK_SIZE]; 
	BufferView entityBuffer;
};

struct EntityTile
{
	sf::Vector2<uint8_t> position;
	uint16_t type; // Esimerkkin� ovi, joka vie toiseen huoneeseen. 1 voi olla ovi, 2 kyltti ja jne...
	uint16_t actionId; //Esim huoneen id johon ovi voi vide� tai vaikkapa kyltin tekstin indeksi listaan, joka on p�in vattua.
	uint16_t textureID; //duh itsess��n selv� jo.
	uint8_t frameCount; // kuinka monta animaatio framea on. 0 jos ei ole kuvaa. 1 jos on staattinen objekti.
	float animationSpeed;
};

/*

Iso vertex array vois toimia johon joka frame lis�t��n ne. V�h� niikuin batching systeemi.


Enitty tilejen tallentaminen yhteen isoon listaan ei ehk� toimi, koska joudumme sorttaamaan sit� koko ajan, kun poistamme
 ja lis��mme asioita. Kysy chatgpt:lt� infoa t�st�. 


 Yksi vaihtoehto on ett� jokaisella chunkilla on aina max size enityj�. 
 Tai jokaisella chunkilla on aina oma lista kaikista asioista enityist� jotka ovat aina n kokoisia maximissaan.



 ---------------

 Ent� jos chunkit ovat editorin olevan k�ynniss� ramissa per chunk vector listoina.
 Sitten kun tallennamme niin k�ymme chunkit l�vitse kerrallaan ja lis��mme sen sitten uuteen isoon listaan.

*/