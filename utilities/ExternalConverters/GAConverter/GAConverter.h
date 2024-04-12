/*
 Copyright © 2021-2023  TokiNoBug
This file is part of SlopeCraft.

    SlopeCraft is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SlopeCraft is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SlopeCraft. If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/SlopeCraft/SlopeCraft
    bilibili:https://space.bilibili.com/351429231
*/

#ifndef SCL_GACVTER_GACONVERTER_H
#define SCL_GACVTER_GACONVERTER_H

#include "GACvterDefines.hpp"
#include "sortColor.h"
#include "uiPack.h"

namespace GACvter {

void iFun(Var_t *, const CvterInfo *) noexcept;
void fFun(const Var_t *, const CvterInfo *, double *) noexcept;
void cFun(const Var_t *, const Var_t *, Var_t *, Var_t *,
          const CvterInfo *) noexcept;
void mFun(const Var_t *, Var_t *, const CvterInfo *) noexcept;

[[nodiscard]] mapColor2Gray_LUT_t updateMapColor2GrayLUT();

class GAConverter
    : private heu::SOGA<Var_t, heu::FitnessOption::FITNESS_LESS_BETTER,
                        heu::RecordOption::DONT_RECORD_FITNESS,
                        heu::SelectMethod::Tournament, CvterInfo, iFun, fFun,
                        cFun, mFun> {
  using Base_t =
      heu::SOGA<Var_t, FitnessOption::FITNESS_LESS_BETTER,
                RecordOption::DONT_RECORD_FITNESS,
                heu::SelectMethod::Tournament, CvterInfo, ::GACvter::iFun,
                ::GACvter::fFun, ::GACvter::cFun, ::GACvter::mFun>;

 public:
  GAConverter();

  void setUiPack(const uiPack &) noexcept;

  inline void setUiPtr(void *_uiptr) noexcept {
    this->_args.ui._uiPtr = _uiptr;
  }

  inline void setProgressRangeFun(void (*fPtr)(void *, int, int,
                                               int)) noexcept {
    this->_args.ui.progressRangeSet = fPtr;
  }

  inline void setProgressAddFun(void (*fPtr)(void *, int)) noexcept {
    this->_args.ui.progressAdd = fPtr;
  }

  void setRawImage(const EImage &) noexcept;
  void setSeeds(
      const std::vector<const Eigen::ArrayXX<mapColor_t> *> &) noexcept;

  using Base_t::option;
  using Base_t::setOption;

  void run();

  void resultImage(EImage *);

 private:
  friend void ::GACvter::iFun(Var_t *, const CvterInfo *) noexcept;
  friend void ::GACvter::fFun(const Var_t *, const CvterInfo *,
                              double *) noexcept;
  friend void ::GACvter::cFun(const Var_t *, const Var_t *, Var_t *, Var_t *,
                              const CvterInfo *) noexcept;
  friend void ::GACvter::mFun(const Var_t *, Var_t *,
                              const CvterInfo *) noexcept;

  HEU_MAKE_GABASE_TYPES(Base_t)
  friend class heu::internal::GABase<
      Var_t, double, RecordOption::DONT_RECORD_FITNESS, Gene_t, CvterInfo,
      ::GACvter::iFun, ::GACvter::fFun, ::GACvter::cFun, ::GACvter::mFun>;

  static constexpr std::clock_t reportInterval = 2 * CLOCKS_PER_SEC;

 protected:
  template <class this_t = GAConverter>
  inline void __impl_recordFitness() noexcept {
    Base_t::template __impl_recordFitness<this_t>();

    this->_args.strongMutation = generation() * 2 < option().maxGenerations;
    if (generation() % 10 == 0) {
      std::clock_t curT = std::clock();
      if (curT - args().prevClock >= reportInterval) {
        args().ui.rangeSet(0, option().maxGenerations, generation());
        this->_args.prevClock = curT;
      }
    }
  }
};

}  // namespace GACvter

#endif  // SCL_GACVTER_GACONVERTER_H
