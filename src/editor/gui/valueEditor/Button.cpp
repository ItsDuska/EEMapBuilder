#include "Button.h"
#include <iostream>

void guiButton::BaseButton::calculatePosition(sf::Vector2f& position, sf::Vector2f& windowSize)
{
	sf::FloatRect textBounds = text.getLocalBounds();
	sf::Vector2f fixedboundsSize = textBounds.getSize() * 1.25f;

	sf::Vector2f fixedPosition(
		(windowSize.x - fixedboundsSize.x) * position.x,
		(windowSize.y - fixedboundsSize.y) * position.y
	);

	rect.setPosition(fixedPosition);
	rect.setSize(fixedboundsSize);
    
	text.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
	text.setPosition(fixedPosition + rect.getSize() / 2.0f);
}

void guiButton::BaseButton::setupCore(const sf::Color color, const unsigned int charSize, const std::string& buttonText, const sf::Font& font, std::function<void(const std::string&)> function)
{
	text.setFont(font);
	text.setString(buttonText);
	text.setCharacterSize(charSize);
	text.setFillColor(sf::Color::White);
	//rect.setFillColor(color);
    sf::Uint8 alpha = 235u;
    rect.setFillColor(sf::Color(26u, 25u, 25u, alpha));
    rect.setOutlineColor(sf::Color(200u, 200u, 200u, alpha));



    this->function = function;
}

uint16_t guiButton::strToU16(const std::string& numberInString)
{
    if (numberInString.size() == 0)
    {
        return 0;
    }
    
	return static_cast<uint16_t>(std::stoi(numberInString)) % UINT16_MAX;
}

const sf::FloatRect& guiButton::BaseButton::getRect() const
{
    return rect.getGlobalBounds();
}

guiButton::TextButton::TextButton(sf::Vector2f position,
    sf::Vector2f& windowSize,
    const sf::Color& color,
    const unsigned int charSize,
    const std::string& buttonText,
    const sf::Font& font,
    std::function<void(const std::string&)> function)
{
	originalText = buttonText;
	setupCore(color, charSize, buttonText+"000", font, function);
	calculatePosition(position, windowSize);
	active = false;
    text.setString(buttonText);
}

void guiButton::TextButton::render(sf::RenderTarget& window) const
{
	window.draw(rect);
	window.draw(text);
}

void guiButton::TextButton::handleEvents(ButtonEventInfo& info, const sf::Vector2f& mousePosition)
{
    if (!active)
    {
        if (!this->rect.getGlobalBounds().contains(mousePosition))
        {
            text.setFillColor(sf::Color::White);
            return;
        }

        text.setFillColor(sf::Color::Cyan);
        
        if (info.leftMousePressed)
        {
            active = true;
            info.leftMousePressed = false;
            text.setFillColor(sf::Color::Green);
        }
        else
        {
            return;
        }
    }

    bool textChanged = false;

    // Handle text input when the box is active
    if (info.textEntered)
    {
        if (std::isdigit(static_cast<char>(info.unicode)) && userInput.size() < 5)
        {
            userInput += static_cast<char>(info.unicode);
            textChanged = true;
        }
        else if (info.unicode == '\b' && !userInput.empty())
        {
            userInput.pop_back();
            textChanged = true;
        }
    }
    else if (info.keyPressed && info.key == sf::Keyboard::Enter)
    {
        active = false;
        function(userInput);
    }

    if (!textChanged)
    {
        return;
    }

    text.setString(originalText + userInput);
}

void guiButton::TextButton::updateText(const uint16_t tilesAttribute)
{
    userInput = std::to_string(tilesAttribute);
    text.setString(originalText + userInput);
}

bool guiButton::TextButton::isActive() const
{
    return active;
}
