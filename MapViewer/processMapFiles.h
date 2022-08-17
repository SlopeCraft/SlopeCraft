#ifndef PROCESSMAPFILES_H
#define PROCESSMAPFILES_H

#include <stdint.h>
#include <vector>

bool uncompress_map_file(const char * filename,std::vector<uint8_t>*const dest);

#endif // PROCESSMAPFILES_H
