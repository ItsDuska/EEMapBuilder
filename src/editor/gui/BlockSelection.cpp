#include "BlockSelection.h"


void BlockSelection::constructElements()
{



    int tileSize = 32;

    // M‰‰ritet‰‰n, montako palikkaa mahtuu yhteen riviin ja sarakkeeseen
    int blocksPerRow = static_cast<int>(baseWindowSize.x) / tileSize;
    int blocksPerCol = static_cast<int>(baseWindowSize.y) / tileSize;

    // Oletetaan, ett‰ spriteSheetStatic sis‰lt‰‰ 'n' palikkaa
    int numBlocks = blocksPerRow * blocksPerCol;

    std::vector<sf::Vertex> vertices;
    vertices.reserve(numBlocks * 4); // Jokainen palikka tarvitsee 4 vertexi‰

    for (int i = 0; i < blocksPerCol; ++i) {
        for (int j = 0; j < blocksPerRow; ++j) {
            int index = i * blocksPerRow + j;

            if (index >= numBlocks) {
                break;
            }

            sf::Vertex quad[4];

            quad[0].position = sf::Vector2f(j * tileSize, i * tileSize);
            quad[1].position = sf::Vector2f((j + 1) * tileSize, i * tileSize);
            quad[2].position = sf::Vector2f((j + 1) * tileSize, (i + 1) * tileSize);
            quad[3].position = sf::Vector2f(j * tileSize, (i + 1) * tileSize);

            quad[0].texCoords = sf::Vector2f(j * tileSize, i * tileSize);
            quad[1].texCoords = sf::Vector2f((j + 1) * tileSize, i * tileSize);
            quad[2].texCoords = sf::Vector2f((j + 1) * tileSize, (i + 1) * tileSize);
            quad[3].texCoords = sf::Vector2f(j * tileSize, (i + 1) * tileSize);

            vertices.push_back(quad[0]);
            vertices.push_back(quad[1]);
            vertices.push_back(quad[2]);
            vertices.push_back(quad[3]);
        }
    }

    // Luo VertexBuffer ja t‰yt‰ se verticeill‰
    staticBlockBuffer.create(vertices.size());
    staticBlockBuffer.update(vertices.data());
}


void BlockSelection::draw(sf::RenderTarget& target,sf::Texture& spriteSheetStatic, sf::Texture& spriteSheetAnimated) const
{
	target.draw(background);

	
	//if tab blocks
	target.draw(staticBlockBuffer);

	//else
	//target.draw(animatedBlockBuffer);
}
