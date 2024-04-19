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

#include "lossy_compressor.h"

#define heu_NO_OUTPUT
#define heu_USE_THREADS

#include <HeuristicFlow/Genetic>

const double initializeNonZeroRatio = 0.05;

constexpr uint16_t popSize = 50;
uint16_t maxFailTimes = 30;
uint16_t lossy_compressor::maxGeneration = 600;
constexpr double crossoverProb = 0.9;
constexpr double mutateProb = 0.01;
constexpr uint32_t reportRate = 50;

using Var_t = Eigen::ArrayX<uint8_t>;

/*
template <typename Scalar_t, ContainerOption DVO = ContainerOption::Std, bool
isFixedRange = false, Scalar_t MinCT = 0, Scalar_t MaxCT = 1>
*/
struct args_t : public heu::FixedDiscreteBox<Var_t, 0, 2> {
  const TokiColor **src;
  bool allowNaturalCompress;
  size_t maxHeight;
  const lossy_compressor *ptr;
  std::clock_t prevClock;
};

using boxVar_t = typename args_t::Var_t;

static_assert(std::is_same<boxVar_t, Var_t>::value, "is not same");

void iFun(Var_t *v, const args_t *arg) {
  v->setZero(arg->dimensions());
  for (auto &i : *v) {
    if (heu::randD() <= initializeNonZeroRatio) {
      i = 1 + std::rand() % 2;
    }
  }
}

void fFun(const Var_t *v, const args_t *arg, double *fitness) {
  height_line HL;
  const TokiColor **src = arg->src;
  const bool allowNaturalCompress = arg->allowNaturalCompress;
  float meanColorDiff = HL.make(src, *v, allowNaturalCompress);
  meanColorDiff /= v->size();

  if (HL.maxHeight() > arg->maxHeight) {
    *fitness = double(arg->maxHeight) - double(HL.maxHeight()) - 1.0;
  } else {
    *fitness = 100.0 / (1e-4f + meanColorDiff);
  }
}

class solver_t
    : public heu::SOGA<
          Eigen::ArrayX<uint8_t>, heu::FitnessOption::FITNESS_GREATER_BETTER,
          heu::RecordOption::DONT_RECORD_FITNESS, heu::SelectMethod::Tournament,
          args_t, iFun, fFun, heu::GADefaults<Var_t, args_t>::cFunSwapXs,
          heu::GADefaults<Var_t, args_t>::mFun> {
 public:
  void customOptAfterEachGeneration() {
    if (this->generation() % reportRate == 0) {
      std::clock_t &prevClock = this->_args.prevClock;
      std::clock_t curClock = std::clock();
      if (curClock - prevClock >= CLOCKS_PER_SEC / 2) {
        prevClock = curClock;
        this->_args.ptr->progress_bar.set_range(
            0, lossy_compressor::maxGeneration, this->generation());
      }
    }
  }
};

lossy_compressor::lossy_compressor() : solver{new solver_t{}} {
  solver->setTournamentSize(3);
}

lossy_compressor::~lossy_compressor() {}

void lossy_compressor::setSource(const Eigen::ArrayXi &_base,
                                 std::span<const TokiColor *> src) {
  assert(_base.rows() == static_cast<int64_t>(src.size() + 1));
  source.resize(_base.rows() - 1);

  for (uint16_t idx = 0; idx < _base.rows() - 1; idx++) {
    // std::cout<<"check row"<<idx<<"->"<<uint16_t(src[idx]->Result)<<std::endl;
    source[idx] = src[idx];
  }
  source.shrink_to_fit();

  // std::cerr<<"source set\n";
}

void lossy_compressor::runGenetic(uint16_t maxHeight,
                                  bool allowNaturalCompress) {
  {
    heu::GAOption opt;
    opt.crossoverProb = crossoverProb;
    opt.maxFailTimes = maxFailTimes;
    opt.maxGenerations = maxGeneration;
    opt.mutateProb = mutateProb;
    opt.populationSize = popSize;
    solver->setOption(opt);
  }
  {
    solver_t::ArgsType args;
    args.setDimensions(source.size());
    args.src = source.data();
    args.allowNaturalCompress = allowNaturalCompress;
    args.maxHeight = maxHeight;
    args.ptr = this;
    args.prevClock = std::clock();
    solver->setArgs(args);
  }
  solver->initializePop();

  solver->run();
}

bool lossy_compressor::compress(uint16_t maxHeight, bool allowNaturalCompress) {
  this->progress_bar.set_range(0, maxGeneration, 0);

  // std::cerr<<"Genetic algorithm started\n";
  uint16_t tryTimes = 0;
  maxFailTimes = 30;
  maxGeneration = 200;
  while (tryTimes < 3) {
    this->runGenetic(maxHeight, allowNaturalCompress);
    if (this->resultFitness() <= 0) {
      tryTimes++;
      maxFailTimes = -1;
      maxGeneration *= 2;
    } else
      break;
  }
  return tryTimes < 3;
}

const Eigen::ArrayX<uint8_t> &lossy_compressor::getResult() const {
  return this->solver->result();
}

double lossy_compressor::resultFitness() const {
  return this->solver->bestFitness();
}
