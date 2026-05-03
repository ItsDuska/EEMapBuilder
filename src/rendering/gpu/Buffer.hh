#pragma once
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_log.h>
#include <utility>

namespace gpu {

class Buffer {
public:
  Buffer() = default;
  Buffer(SDL_GPUDevice *device, Uint32 size, SDL_GPUBufferUsageFlags usage,
         SDL_PropertiesID props = 0);

  ~Buffer();

  Buffer(const Buffer &) = delete;
  Buffer &operator=(const Buffer &) = delete;

  Buffer(Buffer &&other) noexcept { *this = std::move(other); }

  Buffer &operator=(Buffer &&other) noexcept {
    device = other.device;
    buffer = other.buffer;
    size = other.size;

    other.buffer = nullptr;
    return *this;
  }

  void createBuffer(SDL_GPUDevice *device, Uint32 size,
                    SDL_GPUBufferUsageFlags usage, SDL_PropertiesID props = 0);

  static void upload(SDL_GPUDevice *device, SDL_GPUBuffer *dstBuffer,
                     const void *data, Uint32 size, Uint32 offset = 0);

  SDL_GPUBuffer *get() const { return buffer; }
  Uint32 getSize() const { return size; }

private:
  SDL_GPUDevice *device = nullptr;
  SDL_GPUBuffer *buffer = nullptr;
  Uint32 size = 0;
};
} // namespace gpu
