#include "MapImageCvter.h"

#include <ExternalConverters/ExternalConverterStaticInterface.h>
#include <ExternalConverters/GAConverter/GAConverter.h>

using namespace libImageCvt;

libMapImageCvt::MapImageCvter::MapImageCvter()
    : gacvter(new GACvter::GAConverter) {}

void libMapImageCvt::MapImageCvter::convert_image(
    const ::SCL_convertAlgo algo, bool dither,
    const heu::GAOption *const opt) noexcept {
  if (algo != ::SCL_convertAlgo::gaCvter) {
    Base_t::convert_image(algo, dither);
    return;
  }
  // dither = false;
  constexpr int seed_num = 5;
  constexpr std::array<::SCL_convertAlgo, seed_num> seed_algos = {
      ::SCL_convertAlgo::RGB, ::SCL_convertAlgo::RGB_Better,
      ::SCL_convertAlgo::Lab94, ::SCL_convertAlgo::HSV, ::SCL_convertAlgo::XYZ};

  std::array<Eigen::ArrayXX<uint8_t>, seed_num> cvtedmap;
  std::vector<const Eigen::ArrayXX<uint8_t> *> seeds(seed_num);

  for (int a = 0; a < seed_num; a++) {
    Base_t::convert_image(seed_algos[a], false);
    cvtedmap[a] = this->mapcolor_matrix();
    seeds[a] = &cvtedmap[a];
  }
  gacvter->setRawImage(this->raw_image());

  gacvter->setOption(*opt);
  gacvter->setSeeds(seeds);

  gacvter->setUiPack(this->ui);

  gacvter->run();

  Eigen::ArrayXX<ARGB> raw_image_cache = this->_raw_image;

  gacvter->resultImage(&this->_raw_image);

  Base_t::convert_image(::SCL_convertAlgo::RGB_Better, dither);

  this->_raw_image = raw_image_cache;
}