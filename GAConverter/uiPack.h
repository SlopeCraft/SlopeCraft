#ifndef UIPACK_H
#define UIPACK_H
#include "GACvterDefines.hpp"
#include "sortColor.h"

#include <HeuristicFlow/SimpleMatrix>

namespace GACvter {

struct uiPack
{
public:
  void *_uiPtr;
  void (*progressRangeSet)(void *, int, int, int);
  void (*progressAdd)(void *, int);
  inline void rangeSet(int a, int b, int c) const noexcept {
    progressRangeSet(_uiPtr, a, b, c);
  }
  inline void add(int d) const noexcept {
    progressAdd(_uiPtr, d);
  }
};

struct CvterInfo
{
  EImage rawImageCache;
  heu::MatrixDynamicSize<sortColor> colorMap;   //  the relationship between pixels and sortColor s
  std::clock_t prevClock;
  bool strongMutation;
  GrayImage edgeFeatureMap;
  std::vector<Var_t> seeds;  //     seeds to initialize population
  std::unordered_map<ARGB, order_t> iniTool;
  uiPack ui;
};

}   //  namespace GACvter

#endif // UIPACK_H
