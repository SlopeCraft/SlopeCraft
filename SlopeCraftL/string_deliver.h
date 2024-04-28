#ifndef SLOPECRAFT_SLOPECRAFTL_WRITESTRINGDELIVER_H
#define SLOPECRAFT_SLOPECRAFTL_WRITESTRINGDELIVER_H
#include "SlopeCraftL.h"
#include <string_view>

namespace SlopeCraft {
inline void write_to_sd(string_deliver* s, std::string_view sv) noexcept {
  if (s == nullptr) {
    return;
  }
  if (!s->is_valid()) {
    return;
  }

  const size_t move_bytes = std::min(sv.size(), s->capacity);
  memcpy(s->data, sv.data(), move_bytes);
  s->is_complete = move_bytes < sv.size();

  if (!s->is_complete) {
    s->data[s->capacity - 1] = '\0';
  }

  s->size = move_bytes;
}
};  // namespace SlopeCraft

#endif  // SLOPECRAFT_SLOPECRAFTL_WRITESTRINGDELIVER_H
