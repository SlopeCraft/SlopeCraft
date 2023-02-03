#include <CLI11.hpp>
#include <iostream>
#include <json.hpp>

#include <fstream>

#include <unordered_map>
#include <vector>

#include <VisualCraftL.h>

using njson = nlohmann::json;
using std::cout, std::endl;

bool sort_fun(const njson::iterator &a, const njson::iterator &b) noexcept {
  {
    const std::string &str_a = a.value().at("class");
    const std::string &str_b = b.value().at("class");

    if (str_a != str_b) {
      return std::less<std::string>()(str_a, str_b);
    }
  }

  return std::less<std::string>()(a.key(), b.key());
}

int main(int argc, char **argv) {
  std::string in, out;

  CLI::App app;

  app.add_option("-i", in)->check(CLI::ExistingFile)->required();
  app.add_option("-o", out)->required();

  CLI11_PARSE(app, argc, argv);

  njson ijo;
  try {
    std::ifstream ifs(in);

    ijo = njson::parse(ifs, nullptr, true, true);
    ifs.close();
  } catch (std::runtime_error re) {
    cout << re.what() << endl;
  }

  std::vector<njson::iterator> objs;
  objs.reserve(ijo.size());

  for (auto it = ijo.begin(); it != ijo.end(); ++it) {
    objs.emplace_back(it);
    const std::string &str_class = it.value().at("class");
    bool ok = true;
    const VCL_block_class_t block_class =
        VCL_string_to_block_class(str_class.c_str(), &ok);
    if (!ok) {
      abort();
      return 1;
    }

    switch (block_class) {
    case VCL_block_class_t::concrete:
    // case VCL_block_class_t::concrete_powder:
    case VCL_block_class_t::glazed_terracotta:
    case VCL_block_class_t::wool:
    case VCL_block_class_t::shulker_box:
    case VCL_block_class_t::terracotta:
      it.value()["background"] = true;
      break;

    default:
      break;
    }
  }

  std::sort(objs.begin(), objs.end(), sort_fun);

  std::ofstream ofs(out);

  ofs << "{\n";

  for (auto it : objs) {
    ofs << '\"' << it.key() << "\":" << it.value() << ',';
  }

  ofs << '}';
  ofs.close();
  return 0;
}