#ifndef SLOPECRAFT_VISUALCRAFTL_PARSERESOURCEPACK_H
#define SLOPECRAFT_VISUALCRAFTL_PARSERESOURCEPACK_H

#include "Resource_tree.h"
#include <Eigen/Dense>

#include <ColorManip/ColorManip.h>

#include <unordered_map>

std::unordered_map<std::string, Eigen::Array<ARGB, Eigen::Dynamic,
                                             Eigen::Dynamic, Eigen::RowMajor>>
folder_to_images(const zipped_folder &src, bool *const error = nullptr,
                 std::string *const error_string = nullptr) noexcept;

#endif // SLOPECRAFT_VISUALCRAFTL_PARSERESOURCEPACK_H