#include "BlockSelection.h"
#include <iostream>

constexpr float cornerOffset = 10.f;

static void createQuad(std::vector<sf::Vertex>& vertices, float xPos, float yPos, int texX, int texY,sf::Vector2f& tileSize, sf::Vector2i& blockSize)
{
    sf::Vertex quad[4];

    float xPosNext = xPos + tileSize.x;
    float yPosNext = yPos + tileSize.y;

    quad[0].position = sf::Vector2f(xPos, yPos);
    quad[1].position = sf::Vector2f(xPosNext, yPos);
    quad[2].position = sf::Vector2f(xPosNext, yPosNext);
    quad[3].position = sf::Vector2f(xPos, yPosNext);

    quad[0].texCoords = sf::Vector2f(texX, texY);
    quad[1].texCoords = sf::Vector2f(texX + blockSize.x, texY);
    quad[2].texCoords = sf::Vector2f(texX + blockSize.x, texY + blockSize.y);
    quad[3].texCoords = sf::Vector2f(texX, texY + blockSize.y);

    vertices.push_back(quad[0]);
    vertices.push_back(quad[1]);
    vertices.push_back(quad[2]);
    vertices.push_back(quad[3]);
}



int BlockSelection::select(sf::Vector2i& mousePosition)
{
    sf::Vector2f mousePosFloat(mousePosition.x, mousePosition.y);

   



    if (!adjustedBounds.contains(mousePosFloat))
    {
        return -1; // ei ole gui:n sis‰ll‰. Palauta error value.
    }

    const GUIBufferData& data = getCurrentElementData();


    sf::Vector2f localMousePos = mousePosFloat - backgroundOffsetPosition;
    localMousePos.x -= cornerOffset;
    localMousePos.y -= cornerOffset;

    //float visibleYPos = localMousePos.y + currentOffset * tileSize.y;
    float visibleYPos = localMousePos.y + currentOffset * (tileSize.y + spacing);

    int column = static_cast<int>(localMousePos.x / (tileSize.x + spacing));
    int row = static_cast<int>(visibleYPos / (tileSize.y + spacing));

    int blockIndex = row * data.sizeInTilesWithOffset.x + column;
    if (currentTab == 1)
    {
        blockIndex++;
    }


    if (blockIndex >= 0 && blockIndex < data.maxSpriteCount)
    {
        //std::cout << "Selected block index: " << blockIndex << std::endl;
        return blockIndex;
    }

    //std::cerr << "Amogus??: " << blockIndex << std::endl;
    return -1;
}

void BlockSelection::constructElements(const std::vector<sf::Vector2i>* animationStartIndices)
{
    constructVBO(guiElements[0],nullptr);
    constructVBO(guiElements[1], animationStartIndices);

    constructVBO(guiElements[3], nullptr);
    // construct objects...
    // construct layered blocks...

    updateScrollOffset(currentOffset);

}



//Tee t‰st‰ sillee ett‰ voit heitt‰‰ vaa uuden bufferin
void BlockSelection::constructVBO(GUIBufferData& data, const std::vector<sf::Vector2i>* animationStartIndices)
{
    

    const sf::Vector2f bgSize = background.getSize();


    const float bgWidth = bgSize.x;
    const float bgHeight = bgSize.y;

    const float blockWithSpacingWidth = tileSize.x + spacing;
    const float blockWithSpacingHeight = tileSize.y + spacing;

    const int blocksPerRow = static_cast<int>((bgWidth - 2 * cornerOffset + spacing) / blockWithSpacingWidth);
    const int blocksPerCol = static_cast<int>((bgHeight - 2 * cornerOffset + spacing) / blockWithSpacingHeight);

    data.sizeInTilesWithOffset = { blocksPerRow, blocksPerCol };

    const int numBlocks = blocksPerRow * blocksPerCol;

    std::vector<sf::Vertex> vertices;
    vertices.reserve(static_cast<size_t>(numBlocks * 4));

    int blockIndex = 0;

    if (animationStartIndices)
    {
        blockIndex++;
    }

    for (int i = 0; i < data.sizeInTiles.y; ++i)
    {
        float yPos = backgroundOffsetPosition.y + cornerOffset + i * blockWithSpacingHeight; 
        float yPosNext = yPos + tileSize.y;

        for (int j = 0; j < blocksPerRow; ++j)
        {
            if (animationStartIndices)
            {
                if (blockIndex >= animationStartIndices->size())
                {
                    break;
                }
                   

                sf::Vector2i texPos = (*animationStartIndices)[blockIndex];

                int texX = texPos.x * blockSize.x;
                int texY = texPos.y * blockSize.y;

                float xPos = backgroundOffsetPosition.x + cornerOffset + j * blockWithSpacingWidth;
                createQuad(vertices, xPos, yPos, texX, texY,tileSize,blockSize);

                blockIndex++;
            }
            else
            {
                int currentTextureIndex = i * blocksPerRow + j + 1;

                if (currentTextureIndex >= data.maxSpriteCount)
                {
                    break;
                }
                   

                int texX = ((currentTextureIndex - 1) % data.sizeInTiles.x) * blockSize.x;
                int texY = ((currentTextureIndex - 1) / data.sizeInTiles.x) * blockSize.y;

                float xPos = backgroundOffsetPosition.x + cornerOffset + j * blockWithSpacingWidth;
                createQuad(vertices, xPos, yPos, texX, texY, tileSize, blockSize);
            }
        }
    }

    data.buffer.setPrimitiveType(sf::PrimitiveType::Quads);
    data.buffer.setUsage(sf::VertexBuffer::Usage::Static);

    view.count = vertices.size();

    data.buffer.create(vertices.size());
    data.buffer.update(vertices.data());
}

void BlockSelection::constructGUIText(sf::Font& font)
{
    const char* TabTexts[MAX_TAB_COUNT] = {
        "Static Blocks",
        "Animated Blocks",
        "Objects",
        "Layered Blocks" 
    };

    const sf::Vector2f position = background.getPosition();
    const sf::Vector2f size = background.getSize();

    float totalTextWidth = 0.0f;
    for (int i = 0; i< MAX_TAB_COUNT; i++)
    {
        sf::Text& text = displayTabText[i];
        text.setFont(font);
        text.setString(TabTexts[i]);
        totalTextWidth += text.getGlobalBounds().width;
    }

    displayTabText[0].setFillColor(sf::Color::Cyan);

    const float totalSpacing = (MAX_TAB_COUNT - 1) * 10.0f;
    const float totalWidth = totalTextWidth + totalSpacing;

    const float availableSpace = size.x - totalWidth;
    const float padding = availableSpace / (MAX_TAB_COUNT + 1);

    const float startX = position.x + padding;
    
    float currentX = startX;
    for (int i = 0; i < MAX_TAB_COUNT; i++)
    {
        sf::Text& text = displayTabText[i];
        sf::FloatRect bounds = text.getGlobalBounds();
        text.setPosition(currentX, position.y);
        currentX += bounds.width + padding;
    }
}

void BlockSelection::setupBackground(sf::Vector2f windowSize)
{
    //WINDOW ONLY STUFF! NO TOUCHIE!
    float squareSize = windowSize.x / 1.75f;
    float squareX = (windowSize.x - squareSize) / 2.0f;

    sf::Uint8 alpha = 235u;

    background.setSize(sf::Vector2f(squareSize, windowSize.y));
    background.setPosition(squareX, 0);
    background.setFillColor(sf::Color(26u, 25u, 25u, alpha));
    background.setOutlineColor(sf::Color(200u,200u,200u,alpha));
    background.setOutlineThickness(-2);

    sf::FloatRect backgroundBounds = background.getGlobalBounds();

    adjustedBounds = {
        backgroundBounds.left + cornerOffset,
        backgroundBounds.top + cornerOffset,
        backgroundBounds.width - 2 * cornerOffset,
        backgroundBounds.height - 2 * cornerOffset
    };

}

GUIBufferData& BlockSelection::getCurrentElementData()
{
    return guiElements[currentTab];
}


void BlockSelection::awake(sf::Vector2f& windowSize,
    sf::Vector2i& blockSize,
    sf::Vector2f& tileSize,
    PackedTabInformation* packedTabs,
    sf::Font& font,
    sf::Texture** textures)
{
    view.offset = 0;
    view.count = 0;

    currentTab = 0;
    spacing = 5.5f;
    currentOffset = 0;

    this->blockSize = blockSize;

    const float funnieNumber = 1.45f;

    this->tileSize = {tileSize.x / funnieNumber, tileSize.y / funnieNumber };
    setupBackground(windowSize);

    // Static Blocks
    guiElements[0].maxSpriteCount = packedTabs[0].maxSpriteCount;
    guiElements[0].sizeInTiles = packedTabs[0].sizeInTiles;
    guiElements[0].texturePtr = textures[0];

    // Animated Blocks
    guiElements[1].maxSpriteCount = packedTabs[1].maxSpriteCount;
    guiElements[1].sizeInTiles = packedTabs[1].sizeInTiles;
    guiElements[1].texturePtr = textures[1];

    // Objects...
    // data here...

    // Layered Blocks...
    // data here...
    guiElements[3].maxSpriteCount = packedTabs[3].maxSpriteCount;
    guiElements[3].sizeInTiles = packedTabs[3].sizeInTiles;
    guiElements[3].texturePtr = textures[3];

    constructGUIText(font);



    backgroundOffsetPosition = background.getPosition() + sf::Vector2f(0, this->displayTabText[0].getGlobalBounds().height * 1.25f);
}

void BlockSelection::draw(sf::RenderTarget& target)
{
    const GUIBufferData& data = getCurrentElementData();
    globalGUIrenderStates.texture = data.texturePtr;

	target.draw(background);

    
    target.draw(getCurrentElementData().buffer, view.offset, view.count, globalGUIrenderStates);

    for (int i = 0; i < MAX_TAB_COUNT; i++)
    {
        target.draw(this->displayTabText[i]);
    }

    
	//if tab blocks
	//target.draw(staticBlockBuffer);

	//else
	//target.draw(animatedBlockBuffer);
}

void BlockSelection::updateScrollOffset(int& offset)
{
    const GUIBufferData& data = getCurrentElementData();

    offset = std::min(std::max(offset, 0), data.sizeInTiles.y - 1);
    currentOffset = offset;

    view.offset = static_cast<size_t>(offset * (data.sizeInTilesWithOffset.x * 4));
    view.count = data.buffer.getVertexCount() - offset;

    globalGUIrenderStates.transform = sf::Transform::Identity;
    globalGUIrenderStates.transform.translate(0, -offset * (this->tileSize.y+spacing));
}

void BlockSelection::changeTab(int tab)
{
    if (tab >= MAX_TAB_COUNT)
    {
        return;
    }

    displayTabText[currentTab].setFillColor(sf::Color::White);
    displayTabText[tab].setFillColor(sf::Color::Cyan);
    currentTab = tab;
}
