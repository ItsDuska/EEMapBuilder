#include "Application.h"
#include "../Vec2/Vec2.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
// #include <chrono>

Application::Application() {
  /*
  sf::VideoMode currentMode = sf::VideoMode::getDesktopMode();

  window = std::make_unique<sf::RenderWindow>(currentMode, "EEMapBuilder",
  sf::Style::Fullscreen); window->setFramerateLimit(60u);

  sf::Vector2u uWindowSize = window->getSize();
  sf::Vector2f windowSize(static_cast<float>(uWindowSize.x),
  static_cast<float>(uWindowSize.y));
  */
  deltaTime = 0;

  SDL_Init(SDL_INIT_VIDEO);

  Vec2i windowSize{1280, 720};

  context.device =
      SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);
  if (context.device == nullptr) {
    SDL_Log("Failed creating gpu device: %s", SDL_GetError());
    return;
  }

  context.window = SDL_CreateWindow("MapBuilder", windowSize.x, windowSize.y,
                                    SDL_WINDOW_RESIZABLE);
  if (context.window == nullptr) {
    SDL_Log("Failed creating window: %s", SDL_GetError());
    return;
  }

  // renderer = SDL_CreateRenderer(window, nullptr);
  // if (renderer == nullptr) {
  // SDL_Log("Failed creating renderer device: %s", SDL_GetError());
  // return;
  // }

  if (!SDL_ClaimWindowForGPUDevice(context.device, context.window)) {
    SDL_Log("Failed claiming window: %s", SDL_GetError());
    return;
  }

  editor = std::make_unique<EditorCore>(windowSize);

  running = true;
}

Application::~Application() {}

void Application::run() {

  uint64_t lastFrame = 0;
  while (running) {
    updateEvents();

    // const auto start = std::chrono::high_resolution_clock::now();
    update();
    render();

    uint64_t thisFrame = SDL_GetTicks();
    deltaTime = thisFrame - lastFrame;
    lastFrame = thisFrame;
    // const auto end = std::chrono::high_resolution_clock::now();
    // const auto duration =
    // std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    // std::cout << "frame duration: " << duration.count() << "mikro
    // sekuntti\n";

    // double frameTimeInSeconds = duration.count() / 1e6;

    // double fps = 1.0 / frameTimeInSeconds;
    // std::cout << "Frame Time: " << duration << " microseconds\nFPS: " << fps
    // << " pls usko, käytän kuitenki väärää kaavaa tähä :clueless:\n\n";
  }
}

void Application::updateEvents() {
  editor->resetButtonInfo();

  while (SDL_PollEvent(&windowEvent)) {
    switch (windowEvent.type) {
    case SDL_EVENT_QUIT:
      running = false;
      break;
    case SDL_EVENT_KEY_DOWN:
      if (windowEvent.key.key == SDLK_ESCAPE) {
        running = false;
      }
      break;
    }
    editor->events(windowEvent);
  }
}

void Application::update() { editor->update(); }

void Application::render() {
  SDL_GPUCommandBuffer *cmdb = SDL_AcquireGPUCommandBuffer(context.device);
  if (cmdb == NULL) {
    SDL_Log("AcquireGPUCommandBuffer failed: %s", SDL_GetError());
    return;
  }

  SDL_GPUTexture *swapchainTexture;
  if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmdb, context.window,
                                             &swapchainTexture, NULL, NULL)) {
    SDL_Log("WaitAndAcquireGPUSwapchainTexture failed: %s", SDL_GetError());
    return;
  }

  if (swapchainTexture == NULL) {
    SDL_Log("swapchain is null: %s", SDL_GetError());
    return;
  }

  SDL_GPUColorTargetInfo colorTargetInfo = {0};
  colorTargetInfo.texture = swapchainTexture;
  colorTargetInfo.clear_color = (SDL_FColor){0.3f, 0.6f, 0.5f, 1.0f};
  colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
  colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

  SDL_GPURenderPass *renderPass =
      SDL_BeginGPURenderPass(cmdb, &colorTargetInfo, 1, NULL);
  // We draw here

  editor->draw(renderPass);

  // end the pass
  SDL_EndGPURenderPass(renderPass);
  SDL_SubmitGPUCommandBuffer(cmdb);
}
