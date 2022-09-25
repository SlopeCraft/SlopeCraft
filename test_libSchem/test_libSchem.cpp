#include <Schem/Schem.h>

#include <iostream>
#include <vector>

using std::cout, std::endl;

int main() {

  libSchem::Schem schem;

  schem.set_MC_major_version_number(18);
  schem.set_MC_version_number(2865);

  schem.resize(2, 3, 4);

  std::vector<const char *> ids; //= {"minecraft:air", "minecraft:glass"};

  ids.resize(255);
  for (auto &id : ids) {
    id = "minecraft:air";
  }

  ids[0] = "minecraft:air";
  ids[1] = "minecraft:glass";
  ids[2] = "minecraft:white_stained_glass";
  ids[3] = "minecraft:stone";

  for (int idx = 0; idx < schem.size(); idx++) {
    schem(idx) = idx;
  }

  libSchem::litematic_info info;

  if (!schem.export_litematic("test.litematic", info)) {
    cout << "Failed to export." << endl;
  } else {
    cout << "Succeeded to export." << endl;
  }

  return 0;
}