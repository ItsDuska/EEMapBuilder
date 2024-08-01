#include "BlockSelection.h"

BlockSelection::BlockSelection(sf::Vector2f& windowSize, sf::Vector2i& blockSize, sf::Vector2f& staticImageSize, sf::Vector2f& animatedImageSize)
{

    int newX = staticImageSize.x / blockSize.x;
    int newY = staticImageSize.y / blockSize.y;


    int newXX = animatedImageSize.x / blockSize.x;
    int newYY = animatedImageSize.y / blockSize.y;

	staticSize = {newX,newY};
	animatedSize = { newXX,newYY };
    this->blockSize = blockSize;

}

void BlockSelection::constructElements()
{

    //sf::Vector2i blockSpace;
    sf::VertexArray buffer(sf::Quads, staticSize.x * staticSize.y * 4);


    for (int y = 0; y < staticSize.x; ++y)
    {
        for (int x = 0; x < staticSize.y; ++x)
        {
            int index = y * staticSize.x + x;
            int sidebarX = 600 + (index % 4) * blockSize.x;
            int sidebarY = (index / 4) * blockSize.y;

            //const int index = (y * x) * 4;
            sf::Vertex* quad = &buffer[index];

            quad[0].position = sf::Vector2f(sidebarX, sidebarY);
            quad[1].position = sf::Vector2f(sidebarX + blockSize.x, sidebarY);
            quad[2].position = sf::Vector2f(sidebarX + blockSize.x, sidebarY + blockSize.y);
            quad[3].position = sf::Vector2f(sidebarX, sidebarY + blockSize.y);

            quad[0].texCoords = sf::Vector2f(x * blockSize.y, y * blockSize.y);
            quad[1].texCoords = sf::Vector2f((x + 1) * blockSize.x, y * blockSize.y);
            quad[2].texCoords = sf::Vector2f((x + 1) * blockSize.x, (y + 1) * blockSize.y);
            quad[3].texCoords = sf::Vector2f(x * blockSize.x, (y + 1) * blockSize.y);

           

        }
    }

}

void BlockSelection::draw(sf::RenderTarget& target,sf::Texture& spriteSheetStatic, sf::Texture& spriteSheetAnimated) const
{
	target.draw(background);

	
	//if tab blocks
	target.draw(staticBlockBuffer);

	//else
	//target.draw(animatedBlockBuffer);
}
