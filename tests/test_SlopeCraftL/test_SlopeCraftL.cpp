#include <SlopeCraftL/SlopeCraftL.h>

#include <array>
#include <iostream>
#include <vector>

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
  kernel->destroy();

  cout << "testing finished" << endl;

  return 0;
}