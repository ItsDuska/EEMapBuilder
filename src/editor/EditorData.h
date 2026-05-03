#pragma once
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include <SFML/Window/Keyboard.hpp>

struct ButtonEventInfo {
  bool textEntered;
  bool keyPressed;
  bool leftMousePressed;
  sf::Keyboard::Key key;
  sf::Uint32 unicode;
};
