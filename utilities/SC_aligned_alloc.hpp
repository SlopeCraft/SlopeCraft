/*
 Copyright © 2021-2023  TokiNoBug
This file is part of SlopeCraft.

    SlopeCraft is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SlopeCraft is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SlopeCraft. If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/SlopeCraft/SlopeCraft
    bilibili:https://space.bilibili.com/351429231
*/

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

inline void SC_aligned_free(void *ptr) noexcept {
#ifdef _WIN32
  return _aligned_free(ptr);
#else
  return free(ptr);
#endif
}

#endif // SLOPECRAFT_UTITILITIES_ALIGNED_ALLOC_HPP