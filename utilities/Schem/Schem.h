#ifndef SCHEM_SCHEM_H
#define SCHEM_SCHEM_H
#include <type_traits>

#include <Eigen/Dense>

#include <unsupported/Eigen/CXX11/Tensor>

namespace libSchem {
template <int64_t max_block_count = 256> class Schem {
public:
  using ele_t = std::conditional_t<(max_block_count > 256), uint16_t, uint8_t>;

private:
  /// The 3 indices are stored in [x][z][y] col-major, and in minecraft the
  /// best storage is [y][z][x] row-major
  Eigen::Tensor<ele_t, 3> xzy;

public:
  Schem() { xzy.resize(0, 0, 0); }
  Schem(int64_t x, int64_t y, int64_t z) {
    xzy.resize(x, y, z);
    xzy.setZero();
  }

  inline ele_t &operator()(int64_t x, int64_t y, int64_t z) noexcept {
    return xzy(x, z, y);
  }

  inline const ele_t &operator()(int64_t x, int64_t y,
                                 int64_t z) const noexcept {
    return xzy(x, z, y);
  }

  inline int64_t x_range() const noexcept { return xzy.dimension(0); }
  inline int64_t y_range() const noexcept { return xzy.dimension(2); }
  inline int64_t z_range() const noexcept { return xzy.dimension(1); }

  inline int64_t size() const noexcept { return xzy.size(); }
};

} // namespace libSchem

#endif // SCHEM_SCHEM_H