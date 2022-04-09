/*
 Copyright © 2021-2022  TokiNoBug
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
    along with SlopeCraft.  If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/ToKiNoBug
    bilibili:https://space.bilibili.com/351429231
*/

#include "lossyCompressor.h"

#define heu_NO_OUTPUT
#define heu_USE_THREADS

#include <HeuristicFlow/Genetic>


const uint8_t mutateMap[3][2]={{1,2},{0,2},{0,1}};
const double initializeNonZeroRatio=0.05;

const uint16_t popSize=50;
uint16_t maxFailTimes=30;
uint16_t LossyCompressor::maxGeneration=600;
const double crossoverProb=0.9;
const double mutateProb=0.01;
const uint32_t reportRate=50;

using Var_t = Eigen::ArrayX<uint8_t>;
using args_t = std::tuple<
size_t,  //dim
const TokiColor **,  // src
bool,    //  allowNaturalCompress
size_t,  //  maxHeight
const LossyCompressor * ,   //  ptr
std::clock_t    //  prevClock
>;

static const size_t idx_Dim=0;
static const size_t idx_Src=1;
static const size_t idx_allowNaturalCompress=2;
static const size_t idx_maxHeight=3;
static const size_t idx_CompressorPtr=4;
static const size_t idx_prevClock=5;

void iFun(Var_t * v,const args_t * arg) {
    v->setZero(std::get<idx_Dim>(*arg));
    for(auto & i : *v) {
        if(heu::randD()<=initializeNonZeroRatio) {
            i=1+std::rand()%2;
        }
    }
}

void fFun(const Var_t * v,const args_t * arg,double * fitness) {
    HeightLine HL;
    const TokiColor ** src=std::get<idx_Src>(*arg);
    const bool allowNaturalCompress=std::get<idx_allowNaturalCompress>(*arg);
    float meanColorDiff=HL.make(src,*v,allowNaturalCompress);
    meanColorDiff/=v->size();

    if(HL.maxHeight()>std::get<idx_maxHeight>(*arg)) {
        *fitness=double(std::get<idx_maxHeight>(*arg))-double(HL.maxHeight())-1.0;
    }
    else {
        *fitness=100.0/(1e-4f+meanColorDiff);
    }
}

void mFun(const Var_t * src,Var_t * v,const args_t*) {
    *v=*src;
    const size_t idx=heu::randD(0,v->size());
    v->operator[](idx)=mutateMap[v->operator[](idx)][std::rand()%2];
}

class solver_t
    : public heu::SOGA<
        Eigen::ArrayX<uint8_t>,
        heu::FitnessOption::FITNESS_GREATER_BETTER,
        heu::RecordOption::DONT_RECORD_FITNESS,
        args_t,
        iFun,fFun,
        heu::GADefaults<Var_t,args_t,heu::ContainerOption::Eigen>::cFunSwapXs
        ,mFun
        >
{
public:

    void customOptAfterEachGeneration() {
        if(this->generation()%reportRate==0) {
            std::clock_t & prevClock=std::get<idx_prevClock>(this->_args);
            std::clock_t curClock=std::clock();
            if(curClock-prevClock>=CLOCKS_PER_SEC/2) {
                prevClock=curClock;
                (*std::get<idx_CompressorPtr>(this->_args)->
                        progressRangeSetPtr)(*(std::get<idx_CompressorPtr>(this->_args)->windPtr),
                                             0,LossyCompressor::maxGeneration,this->generation());
            }
        }
    }


};

LossyCompressor::LossyCompressor() {
    solver=new solver_t;
}

LossyCompressor::~LossyCompressor() {
    delete solver;
}

void LossyCompressor::setSource(const Eigen::ArrayXi & _base,
                                const TokiColor ** src) {
    source.resize(_base.rows()-1);

    for(uint16_t idx=0;idx<_base.rows()-1;idx++) {
        //std::cout<<"check row"<<idx<<"->"<<uint16_t(src[idx]->Result)<<std::endl;
        source[idx]=src[idx];
    }
    source.shrink_to_fit();

    //std::cerr<<"source set\n";
}

void LossyCompressor::runGenetic(uint16_t maxHeight,bool allowNaturalCompress) {
    {
        static heu::GAOption opt;
        opt.crossoverProb=crossoverProb;
        opt.maxFailTimes=maxFailTimes;
        opt.maxGenerations=maxGeneration;
        opt.mutateProb=mutateProb;
        opt.populationSize=popSize;
        solver_t::ArgsType args;
        std::get<idx_Dim>(args)=source.size();
        std::get<idx_Src>(args)=source.data();
        std::get<idx_allowNaturalCompress>(args)=allowNaturalCompress;
        std::get<idx_maxHeight>(args)=maxHeight;
        std::get<idx_CompressorPtr>(args)=this;
        std::get<idx_prevClock>(args)=std::clock();

        solver->setOption(opt);
        solver->setArgs(args);
        solver->initializePop();
    }

    solver->run();
}

bool LossyCompressor::compress(uint16_t maxHeight,bool allowNaturalCompress) {

    (*progressRangeSetPtr)(*windPtr,0,maxGeneration,0);

    //std::cerr<<"Genetic algorithm started\n";
    uint16_t tryTimes=0;
    maxFailTimes=30;
    maxGeneration=200;
    while(tryTimes<3) {
        runGenetic(maxHeight,allowNaturalCompress);
        if(resultFitness()<=0) {
            tryTimes++;
            maxFailTimes=-1;
            maxGeneration*=2;
        }
        else
            break;
    }
    return tryTimes<3;
}

const Eigen::ArrayX<uint8_t> & LossyCompressor::getResult() const {
    return solver->result();
}

double LossyCompressor::resultFitness() const {
    return solver->bestFitness();
}
