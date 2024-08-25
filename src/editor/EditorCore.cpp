#include "EditorCore.h"
#include <iostream>

/*
* Ota mallia dawn of the ratsin python version...
* world builderissa. Sen toteutus on kehno, mutta idea on aika hyvä.
*
* Kunhan optimoit renderöinnin ja maan luonnin nii kaiken pitäs olla fine.
* 
* https://github.com/ItsDuska/Dawn-of-the-Rats/blob/PythonVersion/Koodit/Level_editor.py
*/


EditorCore::EditorCore(sf::Vector2f& windowSize)
    :cells( 12,12 ), tileSize( windowSize.y / cells.x, windowSize.y / cells.y), engine(windowSize,tileSize)
{
    info.guiIndex = 1;
    info.currentTab = 0;
    info.mode = EditMode::IDLE;
    info.mousePosition = {};
    info.mouseActive = false;
    info.offset = { 0,0 };
    info.solidMode = false;
    info.showSolidBlocks = false;
    info.showLines = true;
    info.hardReset = false;
   
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
    

    if (info.hardReset)
    {
        info.hardReset = false;
        return;
    }

    engine.renderMap(window,info.showSolidBlocks);

    if (info.showLines)
    {
        window.draw(lines);
    }

    engine.drawGUI(window,info.activeInventory,info.showEditor);
}

void EditorCore::events(sf::Event& sfEvent)
{
    //info.buttonInfo = {};

    static int temp = 0;
    switch (sfEvent.type)
    {
    case sf::Event::TextEntered:
        info.buttonInfo.unicode = sfEvent.text.unicode;
        info.buttonInfo.textEntered = true;
        break;

    case sf::Event::MouseWheelScrolled:
        if (info.activeInventory)
        {
            info.guiIndex += (sfEvent.mouseWheelScroll.delta > 0) ? -1 : 1;
            engine.getInventory().updateScrollOffset(info.guiIndex);
        }


        break;
    case sf::Event::MouseButtonPressed:
        if (sfEvent.mouseButton.button == sf::Mouse::Left)
        {
            info.mode = EditMode::ADD;
            info.buttonInfo.leftMousePressed = true;
        }
        else if (sfEvent.mouseButton.button == sf::Mouse::Middle)
        {
            info.mode = EditMode::INSPECT;
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

    case sf::Event::KeyPressed:
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
        case sf::Keyboard::Delete:
            temp++;
            if (temp > 120)
            {
                temp = 0;
                engine.hardReset();
                info.hardReset = true;
            }
            break;
        }
        break;


    case sf::Event::KeyReleased:
        info.buttonInfo.keyPressed = true;
        info.buttonInfo.key = sfEvent.key.code;
        //1-9 tabs
        if (((sfEvent.key.code - sf::Keyboard::Num1) | (sf::Keyboard::Num9 - sfEvent.key.code)) >= 0)
        {
            info.currentTab = sfEvent.key.code - static_cast<uint32_t>(sf::Keyboard::Num1);
            if (info.activeInventory)
            {
                engine.getInventory().changeTab(info.currentTab);
            }

            break;
        }

        switch (sfEvent.key.code)
        {
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
        case sf::Keyboard::E:
            if (info.showEditor)
            {
                break;
            }
            info.activeInventory = !info.activeInventory;
            std::cout << "INVENTORY MODE: " << info.activeInventory << "\n";
            break;
        case sf::Keyboard::Space:
            info.showLines = !info.showLines;
            break;
        case sf::Keyboard::Z:
            //AMOGUS
            engine.executeUndoAction(info.offset);
            break;
        case sf::Keyboard::X:
            engine.executeRedoAction(info.offset);
            break;
        case sf::Keyboard::G:
            engine.resetAnimationRandomness();
            break;
        case sf::Keyboard::Q:
            if (!info.activeInventory)
            {
                info.showEditor = !info.showEditor;
            }
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

void EditorCore::resetButtonInfo()
{
    info.buttonInfo = {};
}

void EditorCore::createLines(sf::Vector2f& windowSize)
{
    std::vector<sf::Vertex> vertices;


    const float realWidth = windowSize.x / cells.x;

    for (int i = 0; i <= cells.x; ++i) {
        if (i * tileSize.x >= windowSize.x)
        {
            break;
        }
        vertices.emplace_back(sf::Vector2f(0, i * tileSize.y), sf::Color::Red);
        vertices.emplace_back(sf::Vector2f(cells.x * realWidth, i * tileSize.y), sf::Color::Green);
    }

    for (int i = 0; i <= cells.y*2; ++i) {
        if (i * tileSize.x >= windowSize.x)
        {
            break;
        }
        vertices.emplace_back(sf::Vector2f(i * tileSize.x, 0), sf::Color::Magenta);
        vertices.emplace_back(sf::Vector2f(i * tileSize.x, cells.y * tileSize.x), sf::Color::Cyan);
    }

    lines.create(vertices.size());
    lines.update(vertices.data());
}
