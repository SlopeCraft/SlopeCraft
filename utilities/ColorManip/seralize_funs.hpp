#ifndef SLOPECRAFT_UTILITIES_COLORMANIP_SERIALIZEFUNS_HPP
#define SLOPECRAFT_UTILITIES_COLORMANIP_SERIALIZEFUNS_HPP

#include <cereal/cereal.hpp>
#include <Eigen/Dense>
#include <ColorManip.h>
#include <cereal/types/vector.hpp>

namespace cereal {
template <class archive>
void save(archive &ar, const Eigen::ArrayXX<ARGB> &img) {
  ar(cereal::make_size_tag(img.rows()));
  ar(cereal::make_size_tag(img.cols()));
  ar(cereal::binary_data(img.data(), img.size() * sizeof(ARGB)));
}

template <class archive>
void load(archive &ar, Eigen::ArrayXX<ARGB> &img) {
  Eigen::Index rows{0}, cols{0};

  ar(cereal::make_size_tag(rows));
  ar(cereal::make_size_tag(cols));

  img.resize(rows, cols);
  ar(cereal::binary_data(img.data(), rows * cols * sizeof(ARGB)));
}
}  // namespace cereal

#endif  // SLOPECRAFT_UTILITIES_COLORMANIP_SERIALIZEFUNS_HPP