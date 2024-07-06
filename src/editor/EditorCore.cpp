#include "EditorCore.h"
#include <iostream>

/*
* Ota mallia dawn of the ratsin python version...
* world builderissa. Sen toteutus on kehno, mutta idea on aika hyv�.
*
* Kunhan optimoit render�innin ja maan luonnin nii kaiken pit�s olla fine.
* 
* https://github.com/ItsDuska/Dawn-of-the-Rats/blob/PythonVersion/Koodit/Level_editor.py
*/


EditorCore::EditorCore(sf::Vector2f& windowSize)
    :cells( 32,32 ), tileSize( windowSize.x / cells.x, windowSize.y / cells.y), engine(windowSize,tileSize)
{
    info.guiIndex = 1;
    currentTab = 0;
    info.mode = EditMode::IDLE;
    info.mousePosition = {};
    info.mouseActive = false;
    info.offset = { 0,0 };
    info.solidMode = false;
    info.showSolidBlocks = false;
   
	lines.setPrimitiveType(sf::Lines);
	lines.setUsage(sf::VertexBuffer::Static);

    createLines(windowSize);
    fileName = path + "EpistePng" + extensionName;
    engine.createMap(fileName);
}

EditorCore::~EditorCore()
{
    engine.saveMap(fileName);
}

void EditorCore::update()
{
    engine.update(info);

    if (!info.mouseActive)
    {
        info.mode = EditMode::IDLE;
    }
}

void EditorCore::draw(sf::RenderWindow& window)
{
    if (info.mouseActive)
    {
        info.mousePosition = sf::Mouse::getPosition(window);

        sf::Vector2u size = window.getSize();

        if (info.mousePosition.x > size.x || info.mousePosition.x < 0)
        {
            info.mousePosition.x = 0;
        }

        if (info.mousePosition.y > size.y || info.mousePosition.y < 0)
        {
            info.mousePosition.y = 0;
        }
    }

    engine.render(window);
    window.draw(lines);
}

void EditorCore::events(sf::Event& sfEvent)
{
    switch (sfEvent.type)
    {
    case sf::Event::MouseWheelScrolled:
        info.guiIndex += (sfEvent.mouseWheelScroll.delta > 0) ? 1 : -1;
        break;
    case sf::Event::MouseButtonPressed:
        if (sfEvent.mouseButton.button == sf::Mouse::Left)
        {
            info.mode = EditMode::ADD;
        }
        else if (sfEvent.mouseButton.button == sf::Mouse::Right)
        {
            info.mode = EditMode::DELETE;
        }
        info.mouseActive = true;
       
        break;
    case sf::Event::MouseButtonReleased:
        info.mouseActive = false;
        break;
    case sf::Event::KeyReleased:
        //1-9 tabs
        if (((sfEvent.key.code - sf::Keyboard::Num1) | (sf::Keyboard::Num9 - sfEvent.key.code)) >= 0)
        {
            currentTab = sfEvent.key.code - static_cast<uint32_t>(sf::Keyboard::Num1);
            break;
        }

        switch (sfEvent.key.code)
        {
        case sf::Keyboard::Key::A:
            info.offset.x--;
            break;
        case sf::Keyboard::Key::D:
            info.offset.x++;
            break;
        case sf::Keyboard::Key::W:
            info.offset.y--;
            break;
        case sf::Keyboard::Key::S:
            info.offset.y++;
            break;
        case sf::Keyboard::Key::Tab:
            engine.saveMap(fileName);
            break;
        case sf::Keyboard::LControl:
            info.solidMode = !info.solidMode;
            std::cout << "SOLID MODE: " << info.solidMode << "\n";
            break;
        case sf::Keyboard::LShift:
            info.showSolidBlocks = !info.showSolidBlocks;
            std::cout << "SHOWING MODE: " << info.showSolidBlocks << "\n";
            break;
        default:
            break;
        }

        break;
    case sf::Event::LostFocus:
        std::cout << "lost Focus\n";
        break;

    case sf::Event::GainedFocus:
        std::cout << "gained Focus\n";
        break;
    default:
        break;
    }
}

void EditorCore::createLines(sf::Vector2f& windowSize)
{
    std::vector<sf::Vertex> vertices;

    for (int i = 0; i <= cells.x; ++i) {
        if (i * tileSize.x >= windowSize.x)
        {
            break;
        }
        vertices.emplace_back(sf::Vector2f(0, i * tileSize.y), sf::Color::Cyan);
        vertices.emplace_back(sf::Vector2f(cells.x * tileSize.x, i * tileSize.y), sf::Color::Cyan);
    }

    for (int i = 0; i <= cells.y; ++i) {
        if (i * tileSize.y >= windowSize.y)
        {
            break;
        }
        vertices.emplace_back(sf::Vector2f(i * tileSize.x, 0), sf::Color::Cyan);
        vertices.emplace_back(sf::Vector2f(i * tileSize.x, cells.y * tileSize.x), sf::Color::Cyan);
    }

    lines.create(vertices.size());
    lines.update(vertices.data());
}