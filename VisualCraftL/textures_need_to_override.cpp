#include "textures_need_to_override.h"

const std::string_view VCL_12_grass_texture_names[] = {
    "minecraft:blocks/double_plant_grass_bottom",
    "minecraft:blocks/double_plant_grass_top",
    "minecraft:blocks/grass_side_overlay", "minecraft:blocks/grass_top"};

const size_t VCL_12_grass_texture_name_size =
    sizeof(VCL_12_grass_texture_names) / sizeof(std::string_view);

const std::string_view VCL_12_foliage_texture_names[] = {
    "minecraft:blocks/leaves_acacia", "minecraft:blocks/leaves_big_oak",
    "minecraft:blocks/leaves_birch",  "minecraft:blocks/leaves_jungle",
    "minecraft:blocks/leaves_oak",    "minecraft:blocks/leaves_spruce",
    "minecraft:blocks/vine"};

const size_t VCL_12_foliage_texture_name_size =
    sizeof(VCL_12_foliage_texture_names) / sizeof(std::string_view);

const std::string_view VCL_latest_grass_texture_names[] = {
    "minecraft:block/grass_block_top",
    "minecraft:block/grass_block_side",
    "minecraft:block/grass_block_side_overlay",
    "minecraft:block/grass",
    "minecraft:block/tall_grass_bottom",
    "minecraft:block/tall_grass_top"};

const size_t VCL_latest_grass_texture_name_size =
    sizeof(VCL_latest_grass_texture_names) / sizeof(std::string_view);

const std::string_view VCL_latest_foliage_texture_names[] = {
    "minecraft:block/acacia_leaves",   "minecraft:block/birch_leaves",
    "minecraft:block/dark_oak_leaves", "minecraft:block/jungle_leaves",
    "minecraft:block/mangrove_leaves", "minecraft:block/oak_leaves",
    "minecraft:block/spruce_leaves",   "minecraft:block/vine"};

const size_t VCL_latest_foliage_texture_name_size =
    sizeof(VCL_latest_foliage_texture_names) / sizeof(std::string_view);