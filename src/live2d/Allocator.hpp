#ifndef LIVE2D_APP_ALLOCATOR_HPP
#define LIVE2D_APP_ALLOCATOR_HPP

#include <CubismFramework.hpp>
#include <ICubismAllocator.hpp>

/**
* @brief Custom allocator for Live2D memory
*/
class LAppAllocator : public Csm::ICubismAllocator {
  void* Allocate(const Csm::csmSizeType size);
  void Deallocate(void* memory);

  void* AllocateAligned(const Csm::csmSizeType size, const Csm::csmUint32 alignment);
  void DeallocateAligned(void* aligned_memory);
};

#endif /* LIVE2D_APP_ALLOCATOR_HPP */