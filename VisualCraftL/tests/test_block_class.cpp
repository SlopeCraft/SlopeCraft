#include "VisualCraftL.h"

#include <iostream>

int main(int, char **) {

  const char *arr[] = {"wood",
                       "planks",
                       "leaves",
                       "mushroom",
                       "slab",
                       "wool",
                       "concrete",
                       "terracotta",
                       "glazed_terracotta",
                       "concrete_powder",
                       "shulker_box",
                       "glass",
                       "redstone",
                       "stone",
                       "ore",
                       "clay",
                       "natural",
                       "crafted",
                       "desert",
                       "nether",
                       "the_end",
                       "ocean",
                       "creative_only",
                       "others"};

  static_assert(sizeof(arr) / sizeof(const char *) ==
                (int)VCL_block_class_t::others + 1);

  bool ok = true;
  for (int i = 0; i <= (int)VCL_block_class_t::others; i++) {
    const VCL_block_class_t ret = VCL_string_to_block_class(arr[i], &ok);

    if (!ok || int(ret) != i) {
      std::cout << "Failed at " << arr[i] << " : ok = " << ok
                << ", ret = " << int(i) << std::endl;
      return 1;
    }
  }

  return 0;
}