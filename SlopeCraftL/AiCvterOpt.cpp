#include "AiCvterOpt.h"

using namespace SlopeCraft;

AiCvterOpt::AiCvterOpt() {
    popSize=50;
    maxGeneration=200;
    maxFailTimes=50;
    crossoverProb=0.8;
    mutationProb=0.01;
}
