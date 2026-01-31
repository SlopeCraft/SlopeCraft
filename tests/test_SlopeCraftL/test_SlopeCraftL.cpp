/*
 Copyright © 2021-2026  TokiNoBug
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

#include <SlopeCraftL/SlopeCraftL.h>

#include <array>
#include <cmath>
#include <cstring>
#include <ctime>
#include <iostream>
#include <random>
#include <vector>

std::mt19937 mt(std::hash<time_t>()(std::time(nullptr)));

using std::cout, std::endl;

int main() {
  SlopeCraft::Kernel *const kernel = SlopeCraft::SCL_createKernel();

  if (kernel == nullptr) {
    return 1;
  }

  cout << "current step : " << (int)kernel->queryStep() << endl;

  {
    std::array<bool, 64> midx;
    midx.fill(true);
    std::array<SlopeCraft::AbstractBlock *, 64> blocks;
    for (auto &ptr : blocks) {
      ptr = SlopeCraft::SCL_createBlock();
    }
    blocks[0]->setId("minecraft:glass");

    for (int idx = 1; idx < int(blocks.size()); idx++) {
      blocks[idx]->setId("minecraft:stone");
      blocks[idx]->setVersion((unsigned char)SCL_gameVersion::ANCIENT);
      blocks[idx]->setBurnable(false);
      blocks[idx]->setDoGlow(false);
      blocks[idx]->setEndermanPickable(false);
      blocks[idx]->setNeedGlass(false);
    }
    kernel->setType(::SCL_mapTypes::Slope, SCL_gameVersion::MC19, midx.data(),
                    blocks.data());

    for (auto i : blocks) {
      SlopeCraft::SCL_destroyBlock(i);
    }
  }

  cout << "current step : " << (int)kernel->queryStep() << endl;

  {
    // in col-major
    uint32_t *const img_data = new uint32_t[256 * 256];
    if (img_data == nullptr) {
      return 1;
    }
    // constexpr int mt19937_result_bytes = sizeof(decltype(mt()));

    // generate a random image
    for (int idx = 0; idx < 256 * 256; idx++) {
      img_data[idx] = (0xFFU << 24) | mt();
    }

    kernel->setRawImage(img_data, 256, 256);

    delete[] img_data;
  }
  cout << "current step : " << (int)kernel->queryStep() << endl;

  kernel->convert(::SCL_convertAlgo::RGB, false);
  cout << "current step : " << (int)kernel->queryStep() << endl;

  kernel->exportAsData("./", 0, nullptr, nullptr);

  cout << "current step : " << (int)kernel->queryStep() << endl;

  kernel->build(::SCL_compressSettings::noCompress);

  cout << "current step : " << (int)kernel->queryStep() << endl;

  char return_buffer[1024] = "";
  kernel->exportAsLitematic("test.litematic", "my_litematic", "42",
                            return_buffer);

  if (std::strlen(return_buffer) <= 0) {
    cout << "Succeed in exporting as litematic" << endl;
  } else {
    cout << "Failed to export as litematic, error message : " << return_buffer
         << endl;
  }

  SlopeCraft::SCL_destroyKernel(kernel);
  cout << "testing finished" << endl;
  return 0;
}