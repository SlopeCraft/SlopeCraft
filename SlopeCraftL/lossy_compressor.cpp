/*
 Copyright © 2021-2026  TokiNoBug
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

#include <random>

#include "lossy_compressor.h"

#include <gapp.hpp>
#include <encoding/integer.hpp>


// using gene_type = Eigen::ArrayX<uint8_t>;
using gene_type = gapp::IntegerGene;


class fitness : public gapp::FitnessFunctionBase<gene_type> {
private:
  std::span<const TokiColor*const> source;
  const std::function<bool(uint8_t)>& need_support_from_base_color;

public:
  bool allow_natural_compress{false};
  size_t max_height{0};

  using base_t = gapp::FitnessFunctionBase<gene_type>;

  fitness(std::span<const TokiColor*const> src, const std::function<bool(uint8_t)>& nsfbc) :
    base_t{src.size()}, source{src}, need_support_from_base_color{nsfbc} {
  }

private:
  gapp::FitnessVector invoke(const gapp::Candidate<gene_type>& sol) const override {
    Eigen::ArrayX<uint8_t> v{sol.size()};
    for (size_t i = 0; i < sol.size(); i++) {
      assert(sol[i]>=0);
      assert(sol[i]<=2);
      v[i] = static_cast<uint8_t>(sol[i]);
    }

    height_line HL;
    const float color_diff_sum = HL.make(source.data(), v, allow_natural_compress, need_support_from_base_color);
    const float mean_color_diff = color_diff_sum / static_cast<float>(v.size());

    if (HL.maxHeight() > max_height) {
      return gapp::FitnessVector{static_cast<double>(max_height) - static_cast<double>(HL.maxHeight()) - 1.0};
    }
    return gapp::FitnessVector{100.0 / (1e-4f + mean_color_diff)};
  }
};

lossy_compress_result lossy_compress(const lossy_compress_input& input) {
  const auto& opt = input.genetic_options;
  auto solver = gapp::IntegerGA{opt.population_size};
  solver.algorithm(gapp::algorithm::SingleObjective{gapp::selection::Tournament{3}, gapp::replacement::KeepBest{}});
  solver.max_gen(opt.max_generation);
  solver.crossover_method(gapp::crossover::integer::Uniform{opt.crossover_probability});
  solver.mutation_method(gapp::mutation::integer::Uniform{opt.mutate_probability});
  solver.stop_condition(gapp::stopping::FitnessBestStall{opt.early_stop_patience, 0.0});

  auto fitness_fun = fitness{input.source, input.need_support_from_base_color};
  fitness_fun.max_height = input.max_height;
  fitness_fun.allow_natural_compress = input.allow_natural_compress;
  const auto bound = [&]() {
    std::vector<gapp::Bounds<int64_t>> ret;
    ret.resize(input.source.size(), {0, 2});
    return ret;
  }();
  auto make_init_pop = [&]() {
    gapp::Population<gapp::IntegerGene> pop;
    pop.reserve(opt.population_size);
    std::mt19937 mt{42};
    std::uniform_real_distribution<float> rand{0, 1};
    std::uniform_int_distribution<int> rand_int{1, 2};
    for (size_t pop_i = 0; pop_i < opt.population_size; pop_i++) {
      gapp::Chromosome<int64_t> vec;
      vec.reserve(input.source.size());
      for (size_t blk_i = 0; blk_i < input.source.size(); blk_i++) {
        int64_t val = 0;
        if (rand(mt) < opt.initialize_non_zero_ratio) {
          val = rand_int(mt);
        }
        assert(0<=val and val<=2);
        vec.emplace_back(val);
      }
      pop.emplace_back(vec, gapp::BoundsView{bound});
    }
    return pop;
  };
  const auto solutions = solver.solve(fitness_fun, bound, opt.max_generation, make_init_pop());
  assert(solutions.size()>0);
  auto& final = solutions[0];
  assert(final.fitness.size()==1);
  Eigen::Map<const Eigen::ArrayX<int64_t>> gene{final.data(), static_cast<Eigen::Index>(final.size())};

  return lossy_compress_result{
    .gene = gene.cast<uint8_t>(),
    .fitness = final.fitness[0],
  };
}
