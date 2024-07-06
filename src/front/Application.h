#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "editor/EditorCore.h"


class Application
{
public:
	Application();
	~Application();
	void run();
private:
	std::unique_ptr<sf::RenderWindow> window;
	sf::Event sfEvent;
	int currentState;
	sf::Clock deltaClock;
	std::unique_ptr<EditorCore> editor;
private:
	void updateSFMLEvents();
	void update();
	void render();
};

