#pragma once
#include <SFML/Window/Keyboard.hpp>

struct ButtonEventInfo
{
	bool textEntered;
	bool keyPressed;
	bool leftMousePressed;
	sf::Keyboard::Key key;
	sf::Uint32 unicode;
};