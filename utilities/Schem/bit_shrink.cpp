#include "bit_shrink.h"
#include <cmath>
#include <memory.h>

void shrink_bits(const uint16_t *const src, const size_t src_count,
                 const int block_types,
                 std::vector<uint64_t> *const dest) noexcept {
  if (src == nullptr || dest == nullptr || src_count <= 0 || block_types <= 1) {
    return;
  }

  const int bits_per_element = std::ceil(std::log2(block_types));

  if (bits_per_element > 16) {
    exit(1);
    return;
  }

  static constexpr size_t bits_of_block = 8 * sizeof(uint64_t);

  const int total_bits = bits_per_element * src_count;
  const int bytes_required = std::ceil(total_bits / 8.0);

  const int uint64_t_required =
      std::ceil(bytes_required / float(sizeof(uint64_t)));

  dest->resize(uint64_t_required);
  memset(dest->data(), 0, bytes_required);

  const uint16_t src_mask = uint16_t((1ULL << bits_per_element) - 1);

  for (int srcidx = 0; srcidx < src_count; srcidx++) {
    const uint16_t val = src[srcidx] & src_mask;

    // the idx of bit starts from 0

    // the beginning bit idx of element to be written
    const size_t beg_bit_idx = bits_per_element * srcidx;
    // the last bit idx of element
    const size_t tail_bit_idx = beg_bit_idx + bits_per_element - 1;

    uint64_t &head_block = dest->at(beg_bit_idx / bits_of_block);

    const bool is_cross_block =
        (beg_bit_idx / bits_of_block != tail_bit_idx / bits_of_block);

    if (is_cross_block) {
      uint64_t &tail_block = *(&head_block + 1);
      const size_t tail_block_beg_bit_idx =
          (&tail_block - dest->data()) * bits_of_block;
      const size_t bits_on_tail_block =
          tail_bit_idx - tail_block_beg_bit_idx + 1;

      const size_t bits_on_head_block = bits_per_element - bits_on_tail_block;

      // write on the head block
      size_t write_mask = val;
      write_mask = write_mask >> tail_bit_idx;
      head_block |= write_mask;

      // write on the tail block
      write_mask = val;
      const size_t left_move_bits = bits_of_block - bits_per_element;
      write_mask = write_mask
                   << left_move_bits; // left move sothat val starts from the
                                      // beginning of a block

      write_mask = write_mask << bits_on_head_block;

      tail_block |= write_mask;
    } else {
      const size_t head_block_beg_bit_idx =
          (&head_block - dest->data()) * bits_of_block;
      const size_t left_move_bits =
          (head_block_beg_bit_idx + 64) - tail_bit_idx;

      size_t write_mask = val;
      write_mask = write_mask << left_move_bits;

      head_block |= write_mask;
    }
  }
}

inline bool is_seperator(const char ch) noexcept {
  switch (ch) {
  case '[':
  case ':':
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

  std::vector<int> keychars;
  keychars.reserve(id.size() / 3);

  int num_op_eq = 0;
  int num_sep = 0;

  for (int idx = traits_beg_idx; idx < id.size(); idx++) {
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