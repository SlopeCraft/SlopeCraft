//
// Created by Joseph on 2026/2/25.
//
#include "shader_spv.h"

const uint8_t compute_shader_spv[] = {
#embed SHADER_FILE
};

const size_t compute_shader_spv_len = sizeof(compute_shader_spv);