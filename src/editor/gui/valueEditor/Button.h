#pragma once
#include "editor/EditorData.h"
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

namespace guiButton
{
	//Base class. Do no use this as the actual button.

	uint16_t strToU16(const std::string& numberInString);

	class BaseButton
	{
	public:
		const sf::FloatRect& getRect() const;
	protected:
		void calculatePosition(sf::Vector2f& position, sf::Vector2f& windowSize);
		void setupCore(const sf::Color color,
			const unsigned int charSize,
			const std::string& buttonText,
			const sf::Font& font,
			std::function<void(const std::string&)> function
		);

		sf::Text text;
		sf::RectangleShape rect;
		std::function<void(const std::string&)> function;
	};


	class TextButton : public BaseButton
	{
	public:
		TextButton(sf::Vector2f position,
			sf::Vector2f& windowSize,
			const sf::Color& color,
			const unsigned int charSize,
			const std::string& buttonText,
			const sf::Font& font,
			std::function<void(const std::string&)> function
		);
		void render(sf::RenderTarget& window) const;
		void handleEvents(ButtonEventInfo& sfEvent, const sf::Vector2f& mousePosition);
		void updateText(const uint16_t tilesAttribute);
		bool isActive() const;
	private:
		std::string originalText;
		std::string userInput;
		sf::Vector2f position;
		sf::Vector2f windowSize;
		bool active;
	};

}
