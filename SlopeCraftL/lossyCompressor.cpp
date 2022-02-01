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

#define OptimT_NO_OUTPUT
#define OptimT_DO_PARALLELIZE
#include <OptimTemplates/Genetic>

OptimT_MAKE_GLOBAL

const uchar mutateMap[3][2]={{1,2},{0,2},{0,1}};
const double initializeNonZeroRatio=0.05;

const ushort popSize=50;
ushort maxFailTimes=30;
ushort LossyCompressor::maxGeneration=600;
const double crossoverProb=0.9;
const double mutateProb=0.01;
const uint reportRate=50;

class solver_t
    : public OptimT::SOGA<
        Eigen::ArrayX<uchar>,
        OptimT::FitnessOption::FITNESS_GREATER_BETTER,
        OptimT::RecordOption::DONT_RECORD_FITNESS,
        size_t,  //dim
        const TokiColor **,  // src
        bool,    //  allowNaturalCompress
        size_t,  //  maxHeight
        const LossyCompressor * ,   //  ptr
        std::clock_t    //  prevClock
        >
{
public:
    using Base_t = OptimT::SOGA<
    Eigen::ArrayX<uchar>,
    OptimT::FitnessOption::FITNESS_GREATER_BETTER,
    OptimT::RecordOption::DONT_RECORD_FITNESS,
    size_t,  //dim
    const TokiColor **,  // src
    bool,    //  allowNaturalCompress
    size_t,  //  maxHeight
    const LossyCompressor * ,   //  ptr
    std::clock_t    //  prevClock
    >;

    OptimT_MAKE_GABASE_TYPES

    static const size_t idx_Dim=0;
    static const size_t idx_Src=1;
    static const size_t idx_allowNaturalCompress=2;
    static const size_t idx_maxHeight=3;
    static const size_t idx_CompressorPtr=4;
    static const size_t idx_prevClock=5;

    using Var_t = Eigen::ArrayX<uchar>;
    static void iFun(Var_t * v,const ArgsType * arg) {
        v->setZero(std::get<idx_Dim>(*arg));
        for(auto & i : *v) {
            if(OptimT::randD()<=initializeNonZeroRatio) {
                i=1+std::rand()%2;
            }
        }
    }

    static void fFun(const Var_t * v,const ArgsType * arg,double * fitness) {
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

    static void cFun(const Var_t * p1,const Var_t * p2,
                     Var_t * c1,Var_t * c2,const ArgsType * arg) {
       const size_t dim=std::get<idx_Dim>(*arg);
       const size_t idx=OptimT::randD(0,dim);
       c1->resize(dim,1);
       c2->resize(dim,1);
       c1->segment(0,idx)=((std::rand()%2)?p1:p2)->segment(0,idx);
       c1->segment(idx,dim-idx)=((std::rand()%2)?p1:p2)->segment(idx,dim-idx);
       c2->segment(0,idx)=((std::rand()%2)?p1:p2)->segment(0,idx);
       c2->segment(idx,dim-idx)=((std::rand()%2)?p1:p2)->segment(idx,dim-idx);
    }

    static void mFun(Var_t * v,const ArgsType*) {
        const size_t idx=OptimT::randD(0,v->size());
        v->operator[](idx)=mutateMap[v->operator[](idx)][std::rand()%2];
    }

    static void ooFun(ArgsType* arg,
                      std::list<typename Base_t::Gene>*,
                      size_t generation,
                      size_t,
                      const OptimT::GAOption*) {
        if(generation%reportRate==0) {
            std::clock_t & prevClock=std::get<idx_prevClock>(*arg);
            std::clock_t curClock=std::clock();
            if(curClock-prevClock>=CLOCKS_PER_SEC/2) {
                prevClock=curClock;
                (*std::get<idx_CompressorPtr>(*arg)->
                        progressRangeSetPtr)(*(std::get<idx_CompressorPtr>(*arg)->windPtr),
                                             0,LossyCompressor::maxGeneration,generation);
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

    for(ushort idx=0;idx<_base.rows()-1;idx++) {
        //std::cout<<"check row"<<idx<<"->"<<ushort(src[idx]->Result)<<std::endl;
        source[idx]=src[idx];
    }
    source.shrink_to_fit();

    //std::cerr<<"source set\n";
}

void LossyCompressor::runGenetic(ushort maxHeight,bool allowNaturalCompress) {
    {
        static OptimT::GAOption opt;
        opt.crossoverProb=crossoverProb;
        opt.maxFailTimes=maxFailTimes;
        opt.maxGenerations=maxGeneration;
        opt.mutateProb=mutateProb;
        opt.populationSize=popSize;
        solver_t::ArgsType args;
        std::get<solver_t::idx_Dim>(args)=source.size();
        std::get<solver_t::idx_Src>(args)=source.data();
        std::get<solver_t::idx_allowNaturalCompress>(args)=allowNaturalCompress;
        std::get<solver_t::idx_maxHeight>(args)=maxHeight;
        std::get<solver_t::idx_CompressorPtr>(args)=this;
        std::get<solver_t::idx_prevClock>(args)=std::clock();


        solver->initialize(solver_t::iFun,solver_t::fFun,solver_t::cFun,solver_t::mFun,
                           solver_t::ooFun,opt,args);
    }

    solver->run();
}

bool LossyCompressor::compress(ushort maxHeight,bool allowNaturalCompress) {

    (*progressRangeSetPtr)(*windPtr,0,maxGeneration,0);

    //std::cerr<<"Genetic algorithm started\n";
    ushort tryTimes=0;
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

const Eigen::ArrayX<uchar> & LossyCompressor::getResult() const {
    return solver->result();
}

double LossyCompressor::resultFitness() const {
    return solver->bestFitness();
}
