#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "editor/gui/valueEditor/Button.h"
#include "chunk/ChunkData.h"
#include <variant>

enum class GuiType : char
{
    ANIMATION,
    ENTITIES
};




template<typename Data>
class DataEditor_t
{
public:
    void addButton(sf::Vector2f position, sf::Vector2f& size,
        const sf::Color& color, const unsigned int fontSize,
        const std::string& text, const sf::Font& font, std::function<void(const std::string&)> function);

    void calculateWindow(sf::Vector2f& windowSize);

    void draw(sf::RenderTarget& window);
    void update(ButtonEventInfo& sfEvent, const sf::Vector2f& mousePosition);

    void bringNewData(Data& data);
    Data& getData();

    void updateButtontext(uint16_t data, int indexToButtonBuffer);

    //std::variant<AnimationTile, EntityTile> getData() const;
    //void setData(const std::variant<AnimationTile, EntityTile>& data);

private:
    //std::vector<guiButton::TextButton>& getCurrentGuiButtons();

private:
    Data data;
    sf::RectangleShape frame;
    std::vector<guiButton::TextButton> buttons;
};

class DataEditor
{
public:
    enum buttonDataMembers
    {
        CURRENT_FRAME,
        FRAME_DELAY,
        ELAPSED_FRAMES,
        TYPE,
        ACTION_ID
    };

    DataEditor();
    void init(sf::Vector2f windowSize, sf::Font& font);
    void setupBoth(sf::Font& font);

    void update(ButtonEventInfo& event, sf::Vector2f& mousePosition,int currentTab);
    void render(sf::RenderTarget& window);

    bool updateAnimationTile(AnimationTile* tile);
    bool updateEntityTile(EntityTile* tile);

    void updateCurrenGUIType(int tab);


    AnimationTile& getAnimationTile();
    EntityTile& getEntityTile();

private:
    DataEditor_t<AnimationTile> animation;
    DataEditor_t<EntityTile> entity;
    sf::Vector2f windowSize;
    GuiType currentType;

    //AnimationTile animationTile;
   // EntityTile entityTile;
};