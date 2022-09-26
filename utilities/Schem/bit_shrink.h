/*
 Copyright © 2021-2022  TokiNoBug
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
    along with SlopeCraft.  If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/ToKiNoBug
    bilibili:https://space.bilibili.com/351429231
*/

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

void shrink_bytes_weSchem(const uint16_t *src, const size_t src_count,
                          const int palette_max,
                          std::vector<uint8_t> *const dest) noexcept;

#endif // SCHEM_BITSHRINK_H