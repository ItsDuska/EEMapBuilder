#include "UndoStack.h"

UndoStack::UndoStack(size_t capacity) : capacity(capacity)
{
}

void UndoStack::addAction(const Action& action)
{
    if (undoStack.size() == capacity)
    {
        undoStack.pop();
    }
    undoStack.push(action);

    // Clear the redo stack whenever a new action is added
    while (!redoStack.empty())
    {
        redoStack.pop();
    }
}

bool UndoStack::undo(Action& action)
{
    if (undoStack.empty())
    {
        return false;
    }

    action = undoStack.top();
    undoStack.pop();
    redoStack.push(action);

    return true;
}

bool UndoStack::redo(Action& action)
{
    if (redoStack.empty())
    {
        return false;
    }

    action = redoStack.top();
    redoStack.pop();
    undoStack.push(action);

    return true;
}

void UndoStack::destroyBuffers()
{
    while (!redoStack.empty())
    {
        redoStack.pop();
    }

    while (!undoStack.empty())
    {
        undoStack.pop();
    }
}
