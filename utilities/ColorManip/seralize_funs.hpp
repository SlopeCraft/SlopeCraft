#ifndef SLOPECRAFT_UTILITIES_COLORMANIP_SERIALIZEFUNS_HPP
#define SLOPECRAFT_UTILITIES_COLORMANIP_SERIALIZEFUNS_HPP

#include <cereal/cereal.hpp>
#include <Eigen/Dense>
#include <ColorManip.h>
#include <cereal/types/vector.hpp>

namespace cereal {
template <class archive, typename T>
void save(archive &ar, const Eigen::ArrayXX<T> &img) {
  static_assert(std::is_arithmetic_v<T>, "T must be arithmetic");
  ar(cereal::make_size_tag(img.rows()));
  ar(cereal::make_size_tag(img.cols()));
  ar(cereal::binary_data(img.data(), img.size() * sizeof(T)));
}

template <class archive, typename T>
void load(archive &ar, Eigen::ArrayXX<T> &img) {
  static_assert(std::is_arithmetic_v<T>, "T must be arithmetic");
  Eigen::Index rows{0}, cols{0};

  ar(cereal::make_size_tag(rows));
  ar(cereal::make_size_tag(cols));

  if (rows < 0 || cols < 0) {
    throw std::runtime_error{"Negative size"};
  }

  img.resize(rows, cols);
  ar(cereal::binary_data(img.data(), rows * cols * sizeof(T)));
}
}  // namespace cereal

#endif  // SLOPECRAFT_UTILITIES_COLORMANIP_SERIALIZEFUNS_HPP