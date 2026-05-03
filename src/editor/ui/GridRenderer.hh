#pragma once
#include "rendering/gpu/Buffer.hh"
#include "rendering/gpu/SDLContext.hh"
#include "rendering/gpu/Vertex.hh"
#include <SDL3/SDL_gpu.h>
#include <vector>

namespace ui {
class GridRenderer {
public:
  GridRenderer(gpu::GPUContext *context);
  ~GridRenderer();
  void populateBuffer(gpu::GPUContext *context, Vec2 windowSize,
                      Vec2i cellCount, Vec2 tileSize);
  void draw(SDL_GPURenderPass *renderpass);

private:
  void createPipeline(gpu::GPUContext *context);

  void uploadBuffer(gpu::GPUContext *context,
                    std::vector<gpu::Vertex> &vertices);

private:
  gpu::Buffer buffer;

  SDL_GPUGraphicsPipeline *pipeline = nullptr;
  size_t vertexCount = 0;

  size_t currentBufferSize = 0;
};
} // namespace ui
