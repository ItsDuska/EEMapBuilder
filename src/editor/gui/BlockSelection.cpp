#include "BlockSelection.h"
#include <iostream>

int BlockSelection::select(sf::Vector2i& mousePosition)
{
    sf::Vector2f mousePosFloat(mousePosition.x, mousePosition.y);

    if (!background.getGlobalBounds().contains(mousePosFloat))
    {
        return -1; // ei ole gui:n sisällä. Palauta error value.
    }



    return 0;
}

void BlockSelection::constructElements()
{
    const float cornerOffset = 10.f; 
    const float spacing = 5.5f;

    const sf::Vector2f bgSize = background.getSize();
    const sf::Vector2f bgPosition = background.getPosition() + sf::Vector2f(0, this->displayTabText[0].getGlobalBounds().height * 1.25f);

    const float bgWidth = bgSize.x;
    const float bgHeight = bgSize.y;

    const float blockWithSpacingWidth = tileSize.x + spacing;
    const float blockWithSpacingHeight = tileSize.y + spacing;

    const int blocksPerRow = static_cast<int>((bgWidth - 2 * cornerOffset + spacing) / blockWithSpacingWidth);
    const int blocksPerCol = static_cast<int>((bgHeight - 2 * cornerOffset + spacing) / blockWithSpacingHeight);

    staticBlockSizeInTiles.x = blocksPerRow;
    staticBlockSizeInTiles.y = blocksPerRow;


    const int numBlocks = blocksPerRow * blocksPerCol;

    std::vector<sf::Vertex> vertices;
    vertices.reserve(static_cast<size_t>(numBlocks * 4));

    bool getOutOfJailCard = false;

    sf::Vector2i textureCordOffset(0, 0);

    for (int i = 0; i < blocksPerCol; ++i)
    {
        float yPos = bgPosition.y + cornerOffset + i * blockWithSpacingHeight;  // Y-koordinaatti kerran per rivi
        float yPosNext = yPos + tileSize.y; // Y-koordinaatti seuraavaa vertexiä varten

        for (int j = 0; j < blocksPerRow; ++j)
        {
            int currentTextureIndex = i * blocksPerRow + j + 1;

            if (currentTextureIndex >= maxStaticTextures)
            {
                getOutOfJailCard = true;
                break;
            }

            float xPos = bgPosition.x + cornerOffset + j * blockWithSpacingWidth;  // X-koordinaatti kerran per sarake
            float xPosNext = xPos + tileSize.x; // X-koordinaatti seuraavaa vertexiä varten

            int texX = (currentTextureIndex % staticTextureWidthInTiles) * blockSize.x;
            int texY = (currentTextureIndex / staticTextureWidthInTiles) * blockSize.y;

            sf::Vertex quad[4];

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

        if (getOutOfJailCard)
        {
            break;
        }
    }

    staticBlockBuffer.setPrimitiveType(sf::PrimitiveType::Quads);
    staticBlockBuffer.setUsage(sf::VertexBuffer::Usage::Static);


    view.count = vertices.size();

    staticBlockBuffer.create(vertices.size());
    staticBlockBuffer.update(vertices.data());
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
}


void BlockSelection::awake(sf::Vector2f& windowSize,
    sf::Vector2i& blockSize,
    int maxStaticTextures,
    int maxAniamatedTextures,
    sf::Vector2f& tileSize,
    int staticTextureWidth,
    int animatedTextuereWidth,
    int staticTextureHeight,
    int animatedTextuereHeight,
    sf::Font& font)
{
    view.offset = 0;
    view.count = 0;

    currentTab = 0;


    this->blockSize = blockSize;

    const float funnieNumber = 1.45f;

    this->tileSize = {tileSize.x / funnieNumber, tileSize.y / funnieNumber };
    setupBackground(windowSize);
    this->maxStaticTextures = maxStaticTextures;
    this->maxAniamatedTextures = maxAniamatedTextures;

    staticTextureWidthInTiles = staticTextureWidth;
    animatedTextuereWidthInTiles = animatedTextuereWidth;
    staticTextureHeightInTiles = staticTextureHeight;
    animatedTextuereHeightInTiles = animatedTextuereHeight;

    

    constructGUIText(font);

}

void BlockSelection::draw(sf::RenderTarget& target,sf::Texture& spriteSheetStatic, sf::Texture* spriteSheetAnimated)
{
    staticBufferStates.texture = &spriteSheetStatic;

	target.draw(background);



    target.draw(staticBlockBuffer, view.offset, view.count, staticBufferStates);
    //target.draw(staticBlockBuffer, view.offset, view.count, &spriteSheetStatic,scrollTransformOffset);
    //target.draw(staticBlockBuffer, &spriteSheetStatic);

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
    offset = std::min(std::max(offset, 0), staticTextureHeightInTiles-1);

    view.offset = static_cast<size_t>(offset * (staticBlockSizeInTiles.x * 4));
    view.count = staticBlockBuffer.getVertexCount() - offset;

    staticBufferStates.transform = sf::Transform::Identity;
    staticBufferStates.transform.translate(0, -offset * this->tileSize.y);
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
