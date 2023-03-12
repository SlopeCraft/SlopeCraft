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

#ifndef PROCESSMAPFILES_H
#define PROCESSMAPFILES_H

#include <Eigen/Dense>
#include <stdint.h>
#include <string>
#include <vector>

// bool uncompress_map_file(const char * filename,std::vector<uint8_t>*const
// dest);

// const uint8_t * find_color_begin(const std::vector<uint8_t>&inflated);

bool process_map_file(
    const char *filename,
    Eigen::Array<uint8_t, 128, 128, Eigen::RowMajor> *const dest,
    std::string *const error_info);

#endif // PROCESSMAPFILES_H
