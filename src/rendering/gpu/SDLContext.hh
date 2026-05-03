#pragma once
#include <SDL3/SDL_gpu.h>

namespace gpu {
struct GPUContext {
  SDL_Window *window;
  SDL_GPUDevice *device;
  // ehkä tarvitaan renderer tähän? emt
};

} // namespace gpu
