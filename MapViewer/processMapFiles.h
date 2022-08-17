#ifndef PROCESSMAPFILES_H
#define PROCESSMAPFILES_H

#include <stdint.h>
#include <vector>

bool uncompress_map_file(const char * filename,std::vector<uint8_t>*const dest);

const uint8_t * find_color_begin(const std::vector<uint8_t>&inflated);

#endif // PROCESSMAPFILES_H
