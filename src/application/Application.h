#pragma once
#include <SDL3/SDL.h>
// #include <SDL3/SDL_video.h>
#include "editor/EditorCore.h"
#include "rendering/gpu/SDLContext.hh"
#include <SDL3/SDL_gpu.h>
#include <memory>

class Application {
public:
  Application();
  ~Application();
  void run();

private:
  gpu::GPUContext context;

  SDL_Event windowEvent;
  int currentState;

  bool running;

  uint64_t deltaTime;
  std::unique_ptr<EditorCore> editor;

private:
  void updateEvents();
  void update();
  void render();
};
