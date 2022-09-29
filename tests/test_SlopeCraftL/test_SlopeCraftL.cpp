#include <SlopeCraftL/SlopeCraftL.h>

#include <array>
#include <iostream>
#include <vector>

#include <cmath>
#include <ctime>
#include <random>

std::mt19937 mt(std::hash<time_t>()(std::time(nullptr)));

using std::cout, std::endl;

int main() {

  SlopeCraft::Kernel *const kernel = SlopeCraft::Kernel::create();

  if (kernel == nullptr) {
    return 1;
  }

  cout << "current step : " << kernel->queryStep() << endl;

  {
    std::array<bool, 64> midx;
    midx.fill(true);
    std::array<SlopeCraft::AbstractBlock *, 64> blocks;
    for (auto &ptr : blocks) {
      ptr = SlopeCraft::AbstractBlock::create();
    }
    blocks[0]->setId("minecraft:glass");

    for (int idx = 1; idx < blocks.size(); idx++) {
      blocks[idx]->setId("minecraft:stone");
      blocks[idx]->setVersion(ANCIENT);
      blocks[idx]->setBurnable(false);
      blocks[idx]->setDoGlow(false);
      blocks[idx]->setEndermanPickable(false);
      blocks[idx]->setNeedGlass(false);
    }
    kernel->setType(FileOnly, MC19, midx.data(), blocks.data());

    for (auto i : blocks) {
      i->destroy();
    }
  }

  cout << "current step : " << kernel->queryStep() << endl;

  {
    // in col-major
    uint32_t *const img_data = new uint32_t[256 * 256];
    if (img_data == nullptr) {
      return 1;
    }
    constexpr int mt19937_result_bytes = sizeof(decltype(mt()));

    // generate a random image
    for (int idx = 0; idx < 256 * 256; idx++) {
      img_data[idx] = (0xFFU << 24) & mt();
    }

    kernel->setRawImage(img_data, 256, 256);

    delete[] img_data;
  }
  cout << "current step : " << kernel->queryStep() << endl;

  kernel->convert(::SCL_convertAlgo::RGB, false);
  cout << "current step : " << kernel->queryStep() << endl;

  kernel->exportAsData("./", 0, nullptr, nullptr);

  kernel->destroy();
  cout << "testing finished" << endl;
  return 0;
}