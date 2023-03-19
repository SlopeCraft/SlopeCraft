#include <process_block_id.h>

#include <iostream>

using std::cout, std::endl;

bool run_single_test(std::string_view str) noexcept;

int main(int, char**) {
  const std::string_view str[] = {
      "",
      "stone",
      "dirt[variant=coarse_dirt,snowy=false]",
      "minecraft:stone",
      "minecraft:blue_concrete",
      "minecraft:concrete[color=light_blue]",
      "minecraft:red_sandstone[type=smooth_red_sandstone]",
      "minecraft:dirt[variant=coarse_dirt,snowy=false]",
      "minecraft:mushroom_stem[east=true,west=true,north=true,south=true,up="
      "true,down=true]"};

  for (auto& id : str) {
    if (!run_single_test(id)) {
      return 1;
    }
  }

  return 0;
}

bool run_single_test(std::string_view str) noexcept {
  std::string namespace_name, pure_id;
  std::vector<std::pair<std::string, std::string>> attributes;

  if (!blkid::process_blk_id(str, &namespace_name, &pure_id, &attributes)) {
    return false;
  }

  cout << "namespace name = " << namespace_name << ", pure id = " << pure_id
       << '\n';

  for (auto& pair : attributes) {
    cout << "    " << pair.first << " = " << pair.second << '\n';
  }
  cout << endl;

  return true;
}