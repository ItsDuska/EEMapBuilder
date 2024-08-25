#include "DataEditor.h"
#include <iostream>

/*
sf::Vector2f position,
			sf::Vector2f& windowSize,
			const sf::Color color,
			const unsigned int charSize,
			const std::string& buttonText,
			const sf::Font& font

		  --- ANIMATION ---
		uint16_t currentFrame;
		uint16_t frameDelay;
		uint16_t elapsedFrames;


		   --- ENTITY ---
		uint16_t currentFrame;
		uint16_t frameDelay;
		uint16_t elapsedFrames;
		uint16_t type;					
		uint16_t actionId;

*/
static std::vector<float> calculateSquarePositions(int n, float a, int windowHeight,float p = 0.75f)
{
	std::vector<float> positions;
	float totalHeight = n * a;
	float remainingSpace = windowHeight - totalHeight;

	// p m‰‰r‰‰, kuinka paljon vapaasta tilasta menee p‰‰tyihin verrattuna neliˆiden v‰liin
	float outerSpacing = remainingSpace * p / 2.0f;
	float innerSpacing = remainingSpace * (1 - p) / (n - 1);

	float firstSquareY = outerSpacing;

	for (int i = 0; i < n; ++i) {
		float centerY = firstSquareY + i * (a + innerSpacing) + a / 2.0f;
		float relativeCenterY = centerY / windowHeight;
		positions.push_back(relativeCenterY);
	}

	return positions;
}


template<typename Data>
void DataEditor_t<Data>::addButton(sf::Vector2f position,
	sf::Vector2f& size,
	const sf::Color& color,
	const unsigned int fontSize,
	const std::string & text,
	const sf::Font & font,
	std::function<void(const std::string&)> function)
{

	guiButton::TextButton button(position, size, color, fontSize, text, font, function);
	
	//buttons.emplace_back(position, size, color, fontSize, text, font, function);
	buttons.push_back(button);
}

template<typename Data>
void DataEditor_t<Data>::calculateWindow(sf::Vector2f& windowSize)
{
	float minX = std::numeric_limits<float>::max();
	float minY = std::numeric_limits<float>::max();
	float maxX = 0.f;
	float maxY = 0.f;

	for (const auto& input : buttons)
	{
		const sf::FloatRect& bounds = input.getRect();
		minX = std::min(minX, bounds.left);
		minY = std::min(minY, bounds.top);
		maxX = std::max(maxX, bounds.left + bounds.width);
		maxY = std::max(maxY, bounds.top + bounds.height);
	}

	const float offset = 20.f; // Tai muu haluamasi offset
	float width = (maxX - minX) + 2 * offset;
	float height = (maxY - minY) + 2 * offset;

	// Aseta kehys
	frame.setSize(sf::Vector2f(width, height));
	frame.setPosition(
		(windowSize.x - width) / 2.f,
		(windowSize.y - height) / 2.f
	);
	sf::Uint8 alpha = 235u;
	frame.setFillColor(sf::Color(26u, 25u, 25u, alpha));
	frame.setOutlineColor(sf::Color(200u, 200u, 200u, alpha));
	frame.setOutlineThickness(-2);
}

template<typename Data>
void DataEditor_t<Data>::draw(sf::RenderTarget& window)
{
	window.draw(frame);

	for (const auto& input : buttons)
	{
		input.render(window);
	}
}
template<typename Data>
void DataEditor_t<Data>::update(ButtonEventInfo& sfEvent, const sf::Vector2f& mousePosition)
{
	for (guiButton::TextButton& button : buttons)
	{
		button.handleEvents(sfEvent, mousePosition);
		if (button.isActive())
		{
			break;
		}
	}
}

template<typename Data>
void DataEditor_t<Data>::bringNewData(Data& data)
{
	this->data = data;
}

template<typename Data>
Data& DataEditor_t<Data>::getData()
{
	return data;
}

template<typename Data>
void DataEditor_t<Data>::updateButtontext(uint16_t data, int indexToButtonBuffer)
{
	buttons[indexToButtonBuffer].updateText(data);
}


/*
----------------------------------------------------------------------------------------------------------------------------------------------------------------
*/


DataEditor::DataEditor()
	: currentType(GuiType::ANIMATION)
{
}

void DataEditor::init(sf::Vector2f windowSize, sf::Font& font)
{
	this->windowSize = windowSize;

	setupBoth(font);
}

void DataEditor::setupBoth(sf::Font& font)
{
	sf::Color color(34u, 38u, 65u, 76u);
	const unsigned int FONT_SIZE = 50u;
	AnimationTile &animationTileRef = getAnimationTile();
	EntityTile& entityTileRef = getEntityTile();

	const float estimatedSize = 1.f;

	{ // ANIMATION

		std::vector<float> yPositions = calculateSquarePositions(3, estimatedSize, windowSize.y,0.85f);

		animation.addButton(sf::Vector2f(0.5f, yPositions[0]),
			windowSize,
			color,
			FONT_SIZE,
			"Current Frame: ",
			font,
			[&animationTileRef](const std::string& str)
			{
				animationTileRef.currentFrame = guiButton::strToU16(str);
			}
		);


		animation.addButton(sf::Vector2f(0.5f, yPositions[1]),
			windowSize,
			color,
			FONT_SIZE,
			"Frame Delay: ",
			font,
			[&animationTileRef](const std::string& str)
			{
				animationTileRef.frameDelay = guiButton::strToU16(str);
			}
		);

		animation.addButton(sf::Vector2f(0.5f, yPositions[2]),
			windowSize,
			color,
			FONT_SIZE,
			"Elapsed Frames: ",
			font,
			[&animationTileRef](const std::string& str)
			{
				animationTileRef.elapsedFrames = guiButton::strToU16(str);
			}
		);
	}

	{ // ENTITIES

		std::vector<float> yPositions = calculateSquarePositions(5, estimatedSize, windowSize.y);

		entity.addButton(sf::Vector2f(0.5f, yPositions[0]),
			windowSize,
			color,
			FONT_SIZE,
			"Current Frame: ",
			font,
			[&entityTileRef](const std::string& str)
			{
				entityTileRef.animation.currentFrame = guiButton::strToU16(str);
			}
		);


		entity.addButton(sf::Vector2f(0.5f, yPositions[1]),
			windowSize,
			color,
			FONT_SIZE,
			"Frame Delay: ",
			font,
			[&entityTileRef](const std::string& str)
			{
				entityTileRef.animation.frameDelay = guiButton::strToU16(str);
			}
		);

		entity.addButton(sf::Vector2f(0.5f, yPositions[2]),
			windowSize,
			color,
			FONT_SIZE,
			"Elapsed Frames: ",
			font,
			[&entityTileRef](const std::string& str)
			{
				entityTileRef.animation.elapsedFrames = guiButton::strToU16(str);
			}
		);

		entity.addButton(sf::Vector2f(0.5f, yPositions[3]),
			windowSize,
			color,
			FONT_SIZE,
			"Type: ",
			font,
			[&entityTileRef](const std::string& str)
			{
				entityTileRef.type = guiButton::strToU16(str);
			}
		);

		entity.addButton(sf::Vector2f(0.5f, yPositions[4]),
			windowSize,
			color,
			FONT_SIZE,
			"Action ID: ",
			font,
			[&entityTileRef](const std::string& str)
			{
				entityTileRef.actionId = guiButton::strToU16(str);
			}
		);
	}

	animation.calculateWindow(windowSize);
	entity.calculateWindow(windowSize);
}

void DataEditor::update(ButtonEventInfo& info, sf::Vector2f& mousePosition, int currentTab)
{
	if (currentType == GuiType::ANIMATION)
	{
		animation.update(info,mousePosition);
		return;
	}

	entity.update(info, mousePosition);
}

void DataEditor::render(sf::RenderTarget& window)
{
	if (currentType == GuiType::ANIMATION)
	{
		animation.draw(window);
		return;
	}

	entity.draw(window);
}


// return true if null
bool DataEditor::updateAnimationTile(AnimationTile* tile)
{	
	if (tile == nullptr)
	{
		return true;
	}

	animation.bringNewData(*tile);
	animation.updateButtontext(tile->currentFrame, CURRENT_FRAME);
	animation.updateButtontext(tile->frameDelay, FRAME_DELAY);
	animation.updateButtontext(tile->elapsedFrames, ELAPSED_FRAMES);

	return false;
}

// return true if null
bool DataEditor::updateEntityTile(EntityTile* tile)
{
	if (tile == nullptr)
	{
		return true;
	}

	entity.bringNewData(*tile);
	entity.updateButtontext(tile->animation.currentFrame, CURRENT_FRAME);
	entity.updateButtontext(tile->animation.frameDelay, FRAME_DELAY);
	entity.updateButtontext(tile->animation.elapsedFrames, ELAPSED_FRAMES);
	entity.updateButtontext(tile->type, TYPE);
	entity.updateButtontext(tile->actionId, ACTION_ID);

	return false;
}

void DataEditor::updateCurrenGUIType(int tab)
{
	switch (tab)
	{
	case 1:
		currentType = GuiType::ANIMATION;
		break;
	case 2:
		currentType = GuiType::ENTITIES;
		break;
	default:
		break;
	}
}

AnimationTile& DataEditor::getAnimationTile()
{
	return animation.getData();
}

EntityTile& DataEditor::getEntityTile()
{
	return entity.getData();
}
