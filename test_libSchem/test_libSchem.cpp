#include <Schem/Schem.h>
#include <Schem/bit_shrink.h>

#include <iostream>
#include <vector>

using std::cout, std::endl;

void test_bit_shrink(const uint16_t *const, const size_t u16_num,
                     size_t block_types);

int main() {

  libSchem::Schem schem;

  schem.set_MC_major_version_number(18);
  schem.set_MC_version_number(2865);

  schem.resize(2, 3, 4);

  std::vector<const char *> ids; //= {"minecraft:air", "minecraft:glass"};

  ids.resize(5);
  for (auto &id : ids) {
    id = "minecraft:air";
  }

  ids[0] = "minecraft:air";
  ids[1] = "minecraft:glass";
  ids[2] = "minecraft:white_stained_glass";
  ids[3] = "minecraft:stone";
  ids[4] = "minecraft:lime_stained_glass";

  for (int idx = 0; idx < schem.size(); idx++) {
    schem(idx) = idx % ids.size();
  }

  schem.set_block_id(ids.data(), ids.size());

  libSchem::litematic_info info;

  // test_bit_shrink(&schem(0), schem.size(), schem.block_types());

  if (!schem.export_litematic("test.litematic", info)) {
    cout << "Failed to export." << endl;
  } else {
    cout << "Succeeded to export." << endl;
  }

  return 0;
}

void test_bit_shrink(const uint16_t *const src, const size_t u16_num,
                     size_t block_types) {
  std::vector<uint64_t> shrinked;

  cout << "Before bit shrinking : [";
  for (int idx = 0; idx < u16_num; idx++) {
    cout << src[idx] << ", ";
  }
  cout << "]" << endl;

  cout << "binary before shrinking : \n";
  for (int idx = 0; idx < u16_num; idx++) {
    uint16_t mask = (1ULL << 15);
    for (int bit = 0; bit < 8 * sizeof(uint16_t); bit++) {
      cout << ((src[idx] & mask) ? ('1') : ('0'));
      mask = mask >> 1;
    }
    cout << ' ';
  }
  cout << endl;

  ::shrink_bits(src, u16_num, block_types, &shrinked);

  cout << "binary after shirnking : \n";

  uint8_t *const data = reinterpret_cast<uint8_t *>(shrinked.data());

  for (int64_t idx = 0; idx < shrinked.size() * sizeof(uint64_t); idx++) {
    uint8_t mask = (1 << 7);
    const uint8_t curbyte = data[idx];
    for (int bit = 0; bit < 8; bit++) {
      cout << ((curbyte & mask) ? '1' : '0');
      mask >>= 1;
    }
    cout << ' ';
  }
  cout << endl;
}