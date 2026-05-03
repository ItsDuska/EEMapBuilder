#include "Buffer.hh"

gpu::Buffer::Buffer(SDL_GPUDevice *device, Uint32 size,
                    SDL_GPUBufferUsageFlags usage, SDL_PropertiesID props) {
  createBuffer(device, size, usage);
}

gpu::Buffer::~Buffer() {
  if (buffer) {
    SDL_ReleaseGPUBuffer(device, buffer);
  }
}

void gpu::Buffer::createBuffer(SDL_GPUDevice *device, Uint32 size,
                               SDL_GPUBufferUsageFlags usage,
                               SDL_PropertiesID props) {
  this->device = device;
  this->size = size;

  SDL_GPUBufferCreateInfo info = {.usage = usage, .size = size, .props = props};
  buffer = SDL_CreateGPUBuffer(device, &info);
  if (!buffer) {
    SDL_Log("GPUBuffer creation failed: %s", SDL_GetError());
    return;
  }
}

void gpu::Buffer::upload(SDL_GPUDevice *device, SDL_GPUBuffer *dstBuffer,
                         const void *data, Uint32 size, Uint32 offset) {

  SDL_GPUTransferBuffer *transferBuffer;
  SDL_GPUTransferBufferCreateInfo transferBufferInfo{
      .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
      .size = static_cast<Uint32>(size),
      .props = 0};

  transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferBufferInfo);

  if (!transferBuffer) {
    SDL_Log("Transfer buffer failed: %s", SDL_GetError());
    return;
  }

  SDL_GPUTransferBufferLocation transferLocation{
      .transfer_buffer = transferBuffer, .offset = 0};

  SDL_GPUBufferRegion dstRegion = {.buffer = dstBuffer,
                                   .offset = static_cast<Uint32>(offset),
                                   .size = static_cast<Uint32>(size)};

  void *mapped = SDL_MapGPUTransferBuffer(device, transferBuffer, false);
  SDL_memcpy(mapped, data, size);
  SDL_UnmapGPUTransferBuffer(device, transferBuffer);

  SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(device);
  SDL_GPUCopyPass *pass = SDL_BeginGPUCopyPass(cmd);

  SDL_UploadToGPUBuffer(pass, &transferLocation, &dstRegion, false);

  SDL_EndGPUCopyPass(pass);
  SDL_SubmitGPUCommandBuffer(cmd);

  SDL_ReleaseGPUTransferBuffer(device, transferBuffer);
}
