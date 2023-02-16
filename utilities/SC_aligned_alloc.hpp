#ifndef SLOPECRAFT_UTITILITIES_ALIGNED_ALLOC_HPP
#define SLOPECRAFT_UTITILITIES_ALIGNED_ALLOC_HPP

#include <stdlib.h>

inline void *SC_aligned_alloc(size_t alignment, size_t bytes) noexcept {
#ifdef _WIN32
  return _aligned_malloc(bytes, alignment);
#else
  return aligned_alloc(alignment, bytes);
#endif
}

#endif // SLOPECRAFT_UTITILITIES_ALIGNED_ALLOC_HPP