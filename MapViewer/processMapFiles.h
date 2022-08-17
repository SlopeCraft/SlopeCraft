#ifndef PROCESSMAPFILES_H
#define PROCESSMAPFILES_H

#include <stdint.h>
#include <vector>
#include <Eigen/Dense>
#include <string>

//bool uncompress_map_file(const char * filename,std::vector<uint8_t>*const dest);

//const uint8_t * find_color_begin(const std::vector<uint8_t>&inflated);

bool process_map_file(const char * filename,
                      Eigen::Array<uint8_t,128,128,Eigen::RowMajor>*const dest,
                      std::string *const error_info);

#endif // PROCESSMAPFILES_H
