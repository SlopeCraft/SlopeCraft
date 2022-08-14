#ifndef AICVTEROPT_H
#define AICVTEROPT_H
#include "SlopeCraftL.h"
#include "SCLDefines.h"

#ifndef SCL_CAPI
namespace SlopeCraft{
#endif


struct AiCvterOpt
{
    AiCvterOpt();
    size_t popSize;
    size_t maxGeneration;
    size_t maxFailTimes;
    double crossoverProb;
    double mutationProb;
};

#ifndef SCL_CAPI
}   //  namespace SlopeCraft
#endif

#endif // AICVTEROPT_H
