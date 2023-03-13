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

#include "bit_shrink.h"

#include <memory.h>

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <iostream>
// no flip now
inline uint64_t flip_byte_order(uint64_t val) noexcept {
  uint8_t *const data = reinterpret_cast<uint8_t *>(&val);
  for (int idx = 0; idx < 4; idx++) {
    std::swap(data[idx], data[7 - idx]);
  }
  return val;
}

void bit_shrink_inverse_skip(void *const dest, const int64_t head_skip_bits,
                             const uint16_t *const src, const int64_t src_count,
                             const int64_t bits_per_element) {
  uint64_t *const data = reinterpret_cast<uint64_t *>(dest);
  constexpr int64_t bits_of_block = 8 * sizeof(uint64_t);

  const uint16_t value_mask = (1ULL << (bits_per_element)) - 1;

  // std::cout << "value_mask = " << value_mask << '\n';

  for (int64_t idx = 0; idx < src_count; idx++) {
    const uint16_t value = src[src_count - 1 - idx] & value_mask;
    //::std::cout << "value = " << value << '\n';
    const int64_t head_bit_idx = head_skip_bits + bits_per_element * idx;
    const int64_t tail_bit_idx = head_bit_idx + bits_per_element - 1;

    uint64_t &head_block = data[head_bit_idx / bits_of_block];

    uint64_t head_block_cache = flip_byte_order(head_block);

    const bool is_cross_block =
        (head_bit_idx / bits_of_block) != (tail_bit_idx / bits_of_block);

    if (is_cross_block) {
      uint64_t &tail_block = data[head_bit_idx / bits_of_block + 1];
      uint64_t tail_block_cache = flip_byte_order(tail_block);

      const int64_t tail_bit_num = tail_bit_idx % bits_of_block + 1;
      // const int64_t head_bit_num = bits_per_element - tail_bit_num;

      // write on head cache
      uint64_t write_mask = value;
      write_mask >>= tail_bit_num;
      head_block_cache |= write_mask;
      // write on tail cache
      write_mask = value;
      const int64_t left_move_bits = bits_of_block - tail_bit_num;
      write_mask <<= left_move_bits;
      tail_block_cache |= write_mask;

      head_block = flip_byte_order(head_block_cache);
      tail_block = flip_byte_order(tail_block_cache);
      continue;
    } else {
      const int64_t head_block_start_bit_idx =
          (head_bit_idx / bits_of_block) * bits_of_block;
      const int64_t bits_before_value = head_bit_idx - head_block_start_bit_idx;
      const int64_t bits_after_value =
          bits_of_block - bits_before_value - bits_per_element;
      uint64_t write_mask = value;
      write_mask <<= bits_after_value;
      //::std::cout << "bits_before_value = " << bits_before_value << '\n';
      //::std::cout << "bits_after_value = " << bits_after_value << '\n';

      head_block_cache |= write_mask;
      head_block = flip_byte_order(head_block_cache);
    }
  }
}

void shrink_bits(const uint16_t *const src, const size_t src_count,
                 const int block_types,
                 std::vector<uint64_t> *const dest) noexcept {
  if (src == nullptr || dest == nullptr || src_count <= 0 || block_types <= 1) {
    return;
  }

  const int bits_per_element = std::ceil(std::log2(block_types));

  //::std::cout << "bits_per_element = " << bits_per_element << ::std::endl;

  if (bits_per_element > 16) {
    exit(1);
    return;
  }

  const size_t total_bits = bits_per_element * src_count;
  const size_t bytes_required = std::ceil(total_bits / 8.0);

  const size_t uint64_t_required =
      std::ceil(bytes_required / float(sizeof(uint64_t)));

  dest->resize(uint64_t_required);
  memset(dest->data(), 0, bytes_required);

  const size_t head_skip_bits = bytes_required * 8 - total_bits;
  //::std::cout << "total_bits = " << total_bits << '\n';
  //::std::cout << "bytes_required = " << bytes_required << '\n';
  //::std::cout << "head_skip_bits = " << head_skip_bits << '\n';

  bit_shrink_inverse_skip(dest->data(), head_skip_bits, src, src_count,
                          bits_per_element);

  // reverse bytes
  {
    uint8_t *data = (uint8_t *)dest->data();
    for (int64_t byteid = 0; byteid < int64_t(bytes_required / 2); byteid++) {
      std::swap(data[byteid], data[bytes_required - 1 - byteid]);
    }
  }
}

inline bool is_seperator(const char ch) noexcept {
  switch (ch) {
  case '[':
  case ',':
  case ']':
    return true;
  default:
    return false;
  }
}

bool process_block_id(
    const std::string_view id, std::string *const pure_id,
    std::vector<std::pair<std::string, std::string>> *const traits) {
  pure_id->clear();
  traits->clear();

  if (id.find_first_of('[') != id.find_last_of('[')) {
    // invalid block id
    return false;
  }
  if (id.find_first_of(']') != id.find_last_of(']')) {
    // invalid block id
    return false;
  }

  if (id.find('[') == std::string_view::npos) {
    *pure_id = id;
    return true;
  }

  const int traits_beg_idx = id.find_first_of('[');
  *pure_id = id.substr(0, traits_beg_idx);

  std::vector<int> keychars;
  keychars.reserve(id.size() / 3);

  int num_op_eq = 0;
  int num_sep = 0;

  for (int idx = traits_beg_idx; idx < int(id.size()); idx++) {
    if (id[idx] == '=') {
      keychars.push_back(idx);
      num_op_eq++;
    }
    if (is_seperator(id[idx])) {
      keychars.push_back(idx);
      num_sep++;
    }
  }

  if (num_op_eq + 1 != num_sep) {
    // wrong block id
    return false;
  }

  traits->reserve(num_op_eq);

  for (int op_eq_idx_idx = 0; op_eq_idx_idx < num_op_eq; op_eq_idx_idx++) {
    const int sep_beg_idx = keychars[2 * op_eq_idx_idx];
    const int op_eq_idx = keychars[2 * op_eq_idx_idx + 1];
    const int sep_tail_idx = keychars[2 * op_eq_idx_idx + 2];
    traits->emplace_back();
    const int key_length = op_eq_idx - sep_beg_idx - 1;
    const int value_length = sep_tail_idx - op_eq_idx - 1;

    traits->back().first = id.substr(sep_beg_idx + 1, key_length);
    traits->back().second = id.substr(op_eq_idx + 1, value_length);
  }

  return true;
}

void shrink_bytes_weSchem(const uint16_t *src, const size_t src_count,
                          const int palette_max,
                          std::vector<uint8_t> *const dest) noexcept {
  if (palette_max <= 128) {
    dest->resize(src_count);
    for (size_t idx = 0; idx < src_count; idx++) {
      dest->at(idx) = src[idx] & 0xFF;
    }
    return;
  }

  dest->reserve(src_count * 2);
  dest->clear();

  for (size_t idx = 0; idx < src_count; idx++) {
    uint16_t temp = src[idx];
    if (temp < 128) {
      dest->emplace_back(temp);
    } else {
      uint8_t byte = temp & 0x7F;
      byte |= 0b10000000;
      dest->emplace_back(byte);
      byte = temp / 128;
      dest->emplace_back(byte);
    }
  }
}