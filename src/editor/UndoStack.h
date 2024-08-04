#pragma once
#include <stack>
#include <SFML/System/Vector2.hpp>


struct Action
{
    sf::Vector2i vertexPosition;
    sf::Vector2i chunkPosition;
    sf::Vector2i positionInChunk;
    uint16_t textureIndexOld;
    uint16_t textureIndexCurrent;
    bool solidModeOld;
    bool solidModeCurrent;
};


class UndoStack
{
public:
    UndoStack(size_t capacity);

    void addAction(const Action& action);
    bool undo(Action& action);
    bool redo(Action& action);
private:
    std::stack<Action> undoStack;
    std::stack<Action> redoStack;
    size_t capacity;
};
