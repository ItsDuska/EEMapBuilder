#include "GridRenderer.hh"
#include "rendering/gpu/Buffer.hh"
#include "rendering/gpu/SDLContext.hh"
#include "rendering/gpu/Vertex.hh"
#include <SDL3/SDL_gpu.h>
#include <cstddef>

SDL_GPUShader *loadShader(SDL_GPUDevice *device, char *filename) {
  return NULL;
}

ui::GridRenderer::GridRenderer(gpu::GPUContext *context) {
  createPipeline(context);
}

void ui::GridRenderer::createPipeline(gpu::GPUContext *context) {
  SDL_GPUShader *vs = loadShader(context->device, "line.vert");
  SDL_GPUShader *fs = loadShader(context->device, "line.frag");

  SDL_GPUVertexBufferDescription vbDesc = {
      .slot = 0,
      .pitch = sizeof(gpu::Vertex),
      .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
  };

  SDL_GPUVertexAttribute attrs[2] = {
      // position
      {.location = 0,
       .buffer_slot = 0,
       .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
       .offset = offsetof(gpu::Vertex, position)},
      // color
      {.location = 1,
       .buffer_slot = 0,
       .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
       .offset = offsetof(gpu::Vertex, color)}};

  SDL_GPUVertexInputState vertexInputState = {.vertex_buffer_descriptions =
                                                  &vbDesc,
                                              .num_vertex_buffers = 1,
                                              .vertex_attributes = attrs,
                                              .num_vertex_attributes = 2};

  SDL_GPUColorTargetDescription colorTargetDescription = {};
  colorTargetDescription.format =
      SDL_GetGPUSwapchainTextureFormat(context->device, context->window);

  SDL_GPUGraphicsPipelineTargetInfo targetInfo = {
      .color_target_descriptions = &colorTargetDescription,
      .num_color_targets = 1,
  };

  SDL_GPUGraphicsPipelineCreateInfo info = {
      .vertex_shader = vs,
      .fragment_shader = fs,
      .vertex_input_state = vertexInputState,
      .primitive_type = SDL_GPU_PRIMITIVETYPE_LINELIST,
      .target_info = targetInfo};

  SDL_GPUGraphicsPipeline *pipeline =
      SDL_CreateGPUGraphicsPipeline(context->device, &info);

  SDL_ReleaseGPUShader(context->device, vs);
  SDL_ReleaseGPUShader(context->device, fs);
}

void ui::GridRenderer::populateBuffer(gpu::GPUContext *context, Vec2 windowSize,
                                      Vec2i cellCount, Vec2 tileSize) {
  std::vector<gpu::LineVertex> vertices;

  float gridWidth = windowSize.x;
  float gridHeight = windowSize.y;

  float cellWidth = gridWidth / cellCount.x;
  float cellHeight = gridHeight / cellCount.y;

  for (int row = 0; row <= cellCount.x; ++row) {
    float y = row * tileSize.y;

    if (y > gridHeight)
      break;

    Vec2 left = {0.0f, y};
    Vec2 right = {gridWidth, y};

    vertices.push_back({left, {255, 0, 0, 255}});  // red
    vertices.push_back({right, {0, 255, 0, 255}}); // green
  }

  for (int col = 0; col <= cellCount.y * 2; ++col) {
    float x = col * tileSize.x;

    if (x > gridWidth)
      break;

    Vec2 top = {x, 0.0f};
    Vec2 bottom = {x, gridHeight};

    vertices.push_back({top, {255, 0, 255, 255}});    // magenta
    vertices.push_back({bottom, {0, 255, 255, 255}}); // cyan
  }

  vertexCount = vertices.size();

  const Uint32 bufferSize =
      static_cast<Uint32>(vertexCount * sizeof(gpu::LineVertex));
  if (buffer.get() == nullptr) {
    buffer.createBuffer(context->device, bufferSize,
                        SDL_GPU_BUFFERUSAGE_VERTEX);
  }
  buffer.upload(context->device, buffer.get(), vertices.data(), bufferSize);
}

void ui::GridRenderer::draw(SDL_GPURenderPass *renderpass) {
  SDL_BindGPUGraphicsPipeline(renderpass, pipeline);

  SDL_GPUBufferBinding bufferBinding = {.buffer = buffer.get(), .offset = 0};
  SDL_BindGPUVertexBuffers(renderpass, 0, &bufferBinding, 1);

  SDL_DrawGPUPrimitives(renderpass, vertexCount, 1, 0, 0);
}
