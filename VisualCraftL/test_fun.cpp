#include <omp.h>
#include <png.h>
#include <stdio.h>

#include <filesystem>
#include <iostream>

#include "ColorManip/ColorManip.h"
#include "ParseResourcePack.h"
#include "Resource_tree.h"
#include "VisualCraftL.h"

using std::cout, std::endl, std::string;
void test_VCL_single_image();
void test_VCL_full_zip();
void test_VCL_single_img_in_zip();
void test_VCL_block_model_full_block();

void test_VCL_parse_block_states();
void test_VCL_parse_block_states_many();

void test_resource_pack(const bool texture_only, const bool parse_block_states);

VCL_EXPORT void test_VCL() {
  // test_VCL_full_zip();
  // test_VCL_single_image();
  //  test_VCL_single_img_in_zip();
  // test_VCL_block_model_full_block();
  // test_VCL_parse_block_states();
  // test_VCL_parse_block_states_many();
  test_resource_pack(false, true);
  // test_dereference();
}

void display_folder(const zipped_folder &folder, const int offset = 0) {
  std::string spaces;
  spaces.resize(offset);
  for (char &ch : spaces) {
    ch = ' ';
  }
  cout << "{\n";

  for (const auto &file : folder.files) {
    cout << spaces << "  " << file.first << " : " << file.second.file_size()
         << " bytes\n";
  }

  for (const auto &subfolder : folder.subfolders) {
    cout << spaces << "  " << subfolder.first << " : ";
    display_folder(subfolder.second, offset + 2);
  }
  cout << spaces << "}\n";
}

bool rewrite_png(const char *const filename,
                 const Eigen::Array<ARGB, Eigen::Dynamic, Eigen::Dynamic,
                                    Eigen::RowMajor> &img) {
  png_struct *png =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if (png == NULL) {
    return false;
  }

  png_info *info = png_create_info_struct(png);

  if (info == NULL) {
    png_destroy_write_struct(&png, NULL);
    return false;
  }

  FILE *fp = NULL;
  fp = fopen(filename, "wb");

  if (fp == NULL) {
    png_destroy_write_struct(&png, &info);
    return false;
  }

  png_init_io(png, fp);

  png_set_IHDR(png, info, img.cols(), img.rows(), 8, PNG_COLOR_TYPE_RGB_ALPHA,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);
  png_set_bgr(png);
  png_write_info(png, info);

  for (int r = 0; r < img.rows(); r++) {
    const uint8_t *const rowptr = reinterpret_cast<const uint8_t *>(&img(r, 0));
    png_write_row(png, rowptr);
  }

  png_write_end(png, info);

  png_destroy_write_struct(&png, &info);

  fclose(fp);

  return true;
}

void test_VCL_single_image() {
  const char *filename = "anvil.png";

  FILE *fp = NULL;
  fp = ::fopen(filename, "rb");

  size_t filesize = std::filesystem::file_size(filename);
  cout << "size of png file : " << filesize << endl;
  void *const buffer = malloc(filesize);
  fread(buffer, 1, filesize, fp);

  Eigen::Array<ARGB, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> img;

  parse_png(buffer, filesize, &img);
  img = resize_image_nearest(img, 60, 32);
  /*
  printf("The 1st pixel is : %#X\n", img(0));
  printf("The 2nd pixel is : %#X\n", img(1));
  printf("The 3rd pixel is : %#X\n", img(2));
  */

  printf("The first 4 pixel in hex :\n");
  for (int i = 0; i < 4 * 4; i++) {
    printf("%hX,", ((uint8_t *)img.data())[i]);
  }
  printf("\n\n");

  rewrite_png("test_rewrite.png", img);

  free(buffer);
}

void test_VCL_full_zip() {
  zipped_folder folder = zipped_folder::from_zip("Vanilla_1_19_2.zip");

  bool is_ok;
  std::string error_message;

  std::unordered_map<std::string, Eigen::Array<ARGB, Eigen::Dynamic,
                                               Eigen::Dynamic, Eigen::RowMajor>>
      images = folder_to_images(folder, &is_ok, &error_message);

  cout << error_message << endl;

  cout << "size of images = " << images.size() << endl;

  cout << "Exporting images again..." << endl;

  for (const auto &file : images) {
    rewrite_png(("test/" + file.first).c_str(), file.second);
  }

  cout << "Finished." << endl;
}

void test_VCL_single_img_in_zip() {
  zipped_folder folder = zipped_folder::from_zip("test.zip");
  zipped_folder *const block_dir = folder.subfolder("assets")
                                       ->subfolder("minecraft")
                                       ->subfolder("textures")
                                       ->subfolder("block");
  const zipped_file &file = block_dir->files.find("anvil.png")->second;
  cout << "Found anvil.png" << endl;
  cout << "size of png file : " << file.file_size() << endl;

  Eigen::Array<ARGB, -1, -1, Eigen::RowMajor> img;

  const bool success = parse_png(file.data(), file.file_size(), &img);

  cout << "success = " << success << endl;

  printf("The first 4 pixel in hex :\n");
  for (int i = 0; i < 4 * 4; i++) {
    printf("%hX,", ((uint8_t *)img.data())[i]);
  }
  printf("\n\n");
  rewrite_png("rewrite_anvil.png", img);
}

void export_projection_images(block_model::model m,
                              const std::string filenameprefix) {
  rewrite_png((filenameprefix + "_up.png").data(),
              m.projection_image(block_model::face_idx::face_up));

  rewrite_png((filenameprefix + "_down.png").data(),
              m.projection_image(block_model::face_idx::face_down));
  rewrite_png((filenameprefix + "_east.png").data(),
              m.projection_image(block_model::face_idx::face_east));
  rewrite_png((filenameprefix + "_west.png").data(),
              m.projection_image(block_model::face_idx::face_west));
  rewrite_png((filenameprefix + "_north.png").data(),
              m.projection_image(block_model::face_idx::face_north));
  rewrite_png((filenameprefix + "_south.png").data(),
              m.projection_image(block_model::face_idx::face_south));
}

void test_VCL_block_model_full_block() {
  zipped_folder vanilla = zipped_folder::from_zip("Vanilla_1_19_2.zip");

  Eigen::Array<ARGB, -1, -1, Eigen::RowMajor> texture_gold, texture_glass;

  const auto &images = vanilla.folder_at("assets")
                           ->subfolder("minecraft")
                           ->subfolder("textures")
                           ->subfolder("block")
                           ->files;
  bool success = parse_png(images.at("F48.png").data(),
                           images.at("F48.png").file_size(), &texture_gold);
  if (!success) {
    cout << "\nFailed to parse F48.png" << endl;
    return;
  }
  success = parse_png(images.at("glass.png").data(),
                      images.at("glass.png").file_size(), &texture_glass);

  if (!success) {
    cout << "\nFailed to parse glass.png" << endl;
    return;
  }

  block_model::model model;
  {
    block_model::element element;
    element._from = {0, 0, 0};
    element._to = {16, 16, 16};
    block_model::face_t face;
    face.texture = &texture_glass;
    element.faces.fill(face);
    model.elements.emplace_back(element);
  }
  {
    block_model::element element;
    element._from = {4, 4, 4};
    element._to = {12, 12, 12};
    block_model::face_t face2;
    face2.texture = &texture_gold;

    element.faces.fill(face2);

    model.elements.emplace_back(element);
    ////////////
  }

  export_projection_images(model, "test_block_model/gold_glass");

  cout << "\nsuccess" << endl;
}

void test_VCL_parse_block_states() {
  zipped_folder vanilla = zipped_folder::from_zip("Vanilla_1_19_2.zip");

  auto &blockstates = vanilla.folder_at("assets")
                          ->subfolder("minecraft")
                          ->subfolder("blockstates")
                          ->files;
  zipped_file &file = blockstates.at("prismarine_stairs.json");

  file.append_0_for_c_str();

  bool success;
  resource_json::block_states_variant variants;

  success = resource_json::parse_block_state(
      (const char *)file.data(), (const char *)file.data() + file.file_size(),
      &variants);
  if (!success) {
    printf("Failed to parse json.\n");
    return;
  }

  // variants.LUT;
  printf("size of LUT = %i\n", (int)variants.LUT.size());
  printf("variants.LUT : \n");

  for (const auto &i : variants.LUT) {
    printf("[");
    for (const auto &j : i.first) {
      printf("%s=%s, ", j.key.data(), j.value.data());
    }
    printf("] : {");
    printf("model_name = %s; ", i.second.model_name.data());

    printf("x = %i; y = %i; ", int(i.second.x), int(i.second.y));

    printf("}\n");
  }

  resource_json::state_list sl = {resource_json::state("axis", "x")};

  resource_json::model_pass_t mp = variants.block_model_name(sl);

  printf("\n matched model name = %s.\n", mp.model_name);

  printf("\nrua~\n");
}

void test_VCL_parse_block_states_many() {
  zipped_folder vanilla = zipped_folder::from_zip("Vanilla_1_19_2.zip");

  auto &files = vanilla.folder_at("assets")
                    ->subfolder("minecraft")
                    ->subfolder("blockstates")
                    ->files;
  std::string_view keys[] = {"basalt.json",
                             "air.json",
                             "acacia_sapling.json",
                             "light_gray_wool.json",
                             "melon.json",
                             "moving_piston.json",
                             "nether_sprouts.json",
                             "oak_trapdoor.json",
                             "piston.json",
                             "player_head.json",
                             "polished_granite_stairs.json"};

  constexpr int num_keys = sizeof(keys) / sizeof(keys[0]);

  resource_json::block_states_variant vars[num_keys];

  // zipped_file zfiles[num_keys];

  for (int i = 0; i < num_keys; i++) {
    zipped_file &zfile = files.at(keys[i].data());

    zfile.append_0_for_c_str();

    const bool success = resource_json::parse_block_state(
        (char *)zfile.data(), (char *)zfile.data() + zfile.file_size(),
        vars + i);

    if (!success) {
      printf("\nFailed to parse %s\n", keys[i].data());
    }
  }

  printf("%i tasks finished\n", num_keys);
}

void test_resource_pack(const bool texture_only,
                        const bool parse_block_states) {
  zipped_folder vanilla = zipped_folder::from_zip("Vanilla_1_19_2.zip");

  resource_pack pack;
  double clk;
  clk = omp_get_wtime();
  const bool ok = pack.add_textures(vanilla);
  clk = omp_get_wtime() - clk;
  if (!ok) {
    printf("Failed to parse.\n");
  }

  printf("\n%i pngs parsed in %F ms.\n", int(pack.get_textures().size()),
         clk * 1000);
  /*
  for (const auto &png : pack.get_textures()) {
    printf("%s : [%i, %i]\n", png.first.data(), int(png.second.rows()),
           int(png.second.rows()));
  }
  */

  if (texture_only) {
    printf("\nFinished.\n");
    return;
  }

  clk = omp_get_wtime();
  pack.add_block_models(vanilla);
  clk = omp_get_wtime() - clk;

  printf("\n%i block models parsed in %F ms.\n", int(pack.get_models().size()),
         clk * 1000);

  if (!parse_block_states) {
    printf("\nFinished.\n");
    return;
  }

  clk = omp_get_wtime();
  pack.add_block_states(vanilla);
  clk = omp_get_wtime() - clk;

  printf("\n%i block states parsed in %F ms.\n",
         int(pack.get_block_states().size()), clk * 1000);

  for (const auto &pair : pack.get_block_states()) {
    printf("%s\n", pair.first.data());
  }

  printf("\nFinished.\n");
}

/*
void test_dereference() {
  std::map<string, string> m{
      {"1", "#2"}, {"2", "#3"}, {"A", "#B"}, {"B", "#C"}, {"C", "D"}};

  printf("The origin set is : \n");
  for (const auto &pair : m) {
    printf("{%s, %s}\n", pair.first.data(), pair.second.data());
  }

  dereference_texture_name(m);

  printf("The dereferenced set is : \n");
  for (const auto &pair : m) {
    printf("{%s, %s}\n", pair.first.data(), pair.second.data());
  }
}
*/