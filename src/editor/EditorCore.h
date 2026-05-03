#pragma once
// #include "EditorEngine.h"
#include "editor/ui/GridRenderer.hh"
#include "rendering/gpu/SDLContext.hh"
#include <SDL3/SDL_events.h>
#include <string>
#include <vector>

constexpr int MAX_TABS = 9;

class EditorCore {
public:
  EditorCore(Vec2 &windowSize, gpu::GPUContext *context);
  ~EditorCore();
  void update();
  void draw(SDL_GPURenderPass *renderpass);
  void events(SDL_Event &windowEvent);
  void resetButtonInfo();

private:
  Vec2i cells;
  Vec2 tileSize;

  // EditorEngine engine;
  // EventInfo info;

  std::string fileName;

  // TEMP CONFIGS
  //  change these to their own congif file.
  // int tabCount;
  int tabTextureAmount[MAX_TABS];

  const char *extensionName = ".chunk";
  const std::string path = "data/maps/";

  ui::GridRenderer gridRenderer;
};
