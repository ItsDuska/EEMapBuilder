#pragma once
#include "core/types/Vector.hh"
#include <stack>

struct Action {
  Vec2i mousePosition;
  Vec2i offset;
  uint16_t textureIndexOld;
  uint16_t textureIndexCurrent;
  bool solidModeOld;
  bool solidModeCurrent;
};

class UndoStack {
public:
  UndoStack(size_t capacity);

  void addAction(const Action &action);
  bool undo(Action &action);
  bool redo(Action &action);

  void destroyBuffers();

private:
  std::stack<Action> undoStack;
  std::stack<Action> redoStack;
  size_t capacity;
};
