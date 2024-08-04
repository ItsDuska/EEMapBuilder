#include "BlockSelection.h"


void BlockSelection::constructElements()
{

    const int blocksPerRow = static_cast<int>(baseWindowSize.x) / blockSize.x;
    const int blocksPerCol = static_cast<int>(baseWindowSize.y) / blockSize.y;

    const int numBlocks = blocksPerRow * blocksPerCol;

    std::vector<sf::Vertex> vertices;
    vertices.reserve(numBlocks * 4);

    for (int i = 0; i < blocksPerCol; ++i)
    {
        for (int j = 0; j < blocksPerRow; ++j)
        {
            const int index = i * blocksPerRow + j;

            if (index >= numBlocks)
            {
                break;
            }

            sf::Vertex quad[4];

            quad[0].position = sf::Vector2f(j * blocksPerRow, i * blocksPerCol);
            quad[1].position = sf::Vector2f((j + 1) * blocksPerRow, i * blocksPerCol);
            quad[2].position = sf::Vector2f((j + 1) * blocksPerRow, (i + 1) * blocksPerCol);
            quad[3].position = sf::Vector2f(j * blocksPerRow, (i + 1) * blocksPerCol);

            quad[0].texCoords = sf::Vector2f(j * blockSize.x, i * blockSize.y);
            quad[1].texCoords = sf::Vector2f((j + 1) * blockSize.x, i * blockSize.y);
            quad[2].texCoords = sf::Vector2f((j + 1) * blockSize.x, (i + 1) * blockSize.y);
            quad[3].texCoords = sf::Vector2f(j * blockSize.x, (i + 1) * blockSize.y);

            vertices.push_back(quad[0]);
            vertices.push_back(quad[1]);
            vertices.push_back(quad[2]);
            vertices.push_back(quad[3]);
        }
    }

    staticBlockBuffer.create(vertices.size());
    staticBlockBuffer.update(vertices.data());
}


void BlockSelection::awake(sf::Vector2f& windowSize, sf::Vector2i& blockSize)
{
    this->blockSize = blockSize;
    baseWindowSize = {windowSize.x / 3, windowSize.y};

}

void BlockSelection::draw(sf::RenderTarget& target,sf::Texture& spriteSheetStatic, sf::Texture& spriteSheetAnimated) const
{
	target.draw(background);

	
	//if tab blocks
	target.draw(staticBlockBuffer);

	//else
	//target.draw(animatedBlockBuffer);
}
