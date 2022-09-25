#ifndef SCHEM_BITSHRINK_H
#define SCHEM_BITSHRINK_H

#include <stdint.h>
#include <string>
#include <string_view>
#include <vector>

/**
 * \brief Bitwise compression for litematica
 *
 * \param src The 3D array in y-z-x order
 * \param src_count Number of elements
 * \param block_types Number of block types, used to compute how many bits to
 * store a element
 * \param dest destination
 */
void shrink_bits(const uint16_t *const src, const size_t src_count,
                 const int block_types,
                 std::vector<uint64_t> *const dest) noexcept;

bool process_block_id(
    const std::string_view id, std::string *const pure_id,
    std::vector<std::pair<std::string, std::string>> *const traits);

#endif // SCHEM_BITSHRINK_H