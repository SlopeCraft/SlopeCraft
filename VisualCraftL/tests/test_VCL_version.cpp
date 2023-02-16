#include <VisualCraftL.h>
#include <iostream>
#include <string>
#include <string_view>

int main() {

  std::string_view ver_str_1 = VCL_version_string();

  std::string ver_str_2;

  for (int i = 0; i < 3; i++) {
    ver_str_2 += std::to_string(VCL_version_component(i));
    ver_str_2.push_back('.');
  }
  ver_str_2.pop_back();

  if (ver_str_1 != ver_str_2) {
    std::cout << "Failed." << std::endl;
    return 1;
  }
  std::cout << "Pass." << std::endl;

  return 0;
}