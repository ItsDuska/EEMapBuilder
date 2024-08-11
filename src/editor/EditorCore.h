#pragma once
#include "EditorEngine.h"
//#include "gui/BlockSelection.h"
#include <vector>


constexpr int MAX_TABS = 9;

class EditorCore
{
public:
	EditorCore(sf::Vector2f& windowSize);
	~EditorCore();
	void update();
	void draw(sf::RenderWindow& window);
	void events(sf::Event& sfEvent);

private:
	void createLines(sf::Vector2f& windowSize);

private:
	sf::Vector2i cells;
	sf::Vector2f tileSize;

	EditorEngine engine;
	EventInfo info;
	
	std::string fileName;
	

	//TEMP CONFIGS 
	// change these to their own congif file.
	//int tabCount;
	int tabTextureAmount[MAX_TABS];

	sf::VertexBuffer lines;
	const char* extensionName = ".chunk";
	const std::string path = "data/maps/";

};
