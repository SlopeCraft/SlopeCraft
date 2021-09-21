/*
 Copyright © 2021  TokiNoBug
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

const float gene::unCaculatedSign=-65536*2;
const uchar gene::mutateMap[3][2]={{1,2},{0,2},{0,1}};

const ushort LossyCompressor::popSize=50;
const ushort LossyCompressor::maxFailTimes=30;
ushort LossyCompressor::maxGeneration=600;
const double LossyCompressor::crossoverProb=0.9;
const double LossyCompressor::mutateProb=0.01;
const double LossyCompressor::initializeNonZeroRatio=0.05;

double randD(){
    static std::default_random_engine generator(time(0));
    static std::uniform_real_distribution<double> rander(0,1);
    return rander(generator);
}

gene::gene() {
    fitness=unCaculatedSign;
}

gene::gene(ushort size) {
    DNA.setZero(size);
    fitness=unCaculatedSign;
}

bool gene::isCaculated() const {
    return fitness>unCaculatedSign;
}

float gene::getFitness() const {
    return fitness;
}

ushort gene::size() const {
    return DNA.size();
}

const Eigen::Array<uchar,Eigen::Dynamic,1> & gene::getDNA() const {
    return DNA;
}
void gene::initialize(ushort size) {
    fitness=unCaculatedSign;
    DNA.setZero(size);
}
void gene::caculateFitness(const TokiColor** src,ushort maxHeight,
                           bool allowNaturalCompress) {
    if(isCaculated())return;

    HeightLine HL;
    //std::cerr<<"start to make\n";
    float meanColorDiff=HL.make(src,DNA,allowNaturalCompress);

    meanColorDiff/=size();

    //std::cerr<<"sumColorDiff="<<sumColorDiff<<std::endl;

    if(HL.maxHeight()>maxHeight) {
        fitness=maxHeight-HL.maxHeight()-1;
    } else {
        fitness=100.0/(1e-4f+meanColorDiff);
    }
}
void gene::crossover(gene* a,gene* b,ushort idx) {
    a->DNA.segment(0,idx).swap(b->DNA.segment(0,idx));
    a->fitness=unCaculatedSign;
    b->fitness=unCaculatedSign;
    // 0<idx<length-1
}
void gene::mutate(ushort idx) {
    DNA(idx)=mutateMap[DNA(idx)][rand()%2];
    fitness=unCaculatedSign;
}

#ifdef WITH_QT
LossyCompressor::LossyCompressor(QObject *parent) : QObject(parent)
#else
LossyCompressor::LossyCompressor()
#endif
{
    population.resize(popSize);
}

void LossyCompressor::initialize() {
    //maxGeneration=200;
    population.resize(popSize);
    for(ushort i=0;i<popSize;i++) {
        population[i].initialize(source.size());
        for(ushort r=0;r<source.size();r++) {
            if(randD()<initializeNonZeroRatio)
                population[i].mutate(r);
        }
    }
    //std::cerr<<"population initialized\n";
}

void LossyCompressor::caculateFitness() {
    for(ushort i=0;i<popSize;i++)
        population[i].caculateFitness(&source[0],maxHeight,allowNaturalCompress);
    //std::cerr<<"population fitnesses caculated\n";
}

void LossyCompressor::select() {
    ushort minIdx=0,maxIdx=0;
    float minFitness=population[0].getFitness(),
            maxFitness=population[0].getFitness();
    float temp;
    for(ushort idx=0;idx<popSize;idx++) {
        temp=population[idx].getFitness();
        if(temp>maxFitness) {
            maxFitness=temp;
            maxIdx=idx;
        }
        if(temp<minFitness) {
            minFitness=temp;
            minIdx=idx;
        }
    }
    if(eliteIdx==maxIdx)
        failTimes++;
    else
        failTimes=0;
    eliteIdx=maxIdx;
    population[minIdx]=population[maxIdx];
}

void LossyCompressor::crossover() {
    std::vector<ushort> crossoverQueue;
    crossoverQueue.clear();
    for(ushort i=0;i<popSize;i++) {
        if(i==eliteIdx)
            continue;
        if(randD()<crossoverProb)
            crossoverQueue.push_back(i);
    }
    std::random_shuffle(crossoverQueue.begin(),crossoverQueue.end());
    if(crossoverQueue.size()%2==1)
        crossoverQueue.pop_back();

    for(ushort i=0;i<crossoverQueue.size()/2;i++) {
        ushort a=crossoverQueue[2*i];
        ushort b=crossoverQueue[2*i+1];
        // 1<=idx<=length-2
        ushort idx=int(randD()*65535)%(source.size()-2)+1;

        gene::crossover(&population[a],
                        &population[b],idx);
    }
}

void LossyCompressor::mutate() {
    for(ushort i=0;i<popSize;i++) {
        if(i==eliteIdx)
            continue;
        if(randD()<mutateProb) {
            ushort idx=int(randD()*65535)%(source.size());
            population[i].mutate(idx);
        }
    }
}

void LossyCompressor::runGenetic() {
    initialize();
    failTimes=0;
    generation=0;
    emit progressRangeSet(0,maxGeneration,0);
    while(true) {
        emit keepAwake();
        //std::cerr<<"start!\n";
        caculateFitness();
        //std::cerr<<"caculateFitness\n";
        select();
        //std::cerr<<"select\n";
        if(population[eliteIdx].getFitness()>0&&failTimes>=maxFailTimes) {
            //std::cerr<<"iteration success"<<std::endl;
            break;
        }
        if(generation>=maxGeneration) {
            /*
            if(population[eliteIdx].getFitness()>0)
                std::cerr<<"iteration success"<<std::endl;
            else
                std::cerr<<"iteration failed"<<std::endl;
                */
            break;
        }

        crossover();
        //std::cerr<<"crossover\n";
        mutate();
        //std::cerr<<"mutate\n";
        generation++;
        //std::cerr<<"generation"<<generation<<std::endl;
        emit progressAdd(1);
    }
    std::cerr<<"result fitness="<<getResult().getFitness()<<std::endl;
    emit progressRangeSet(0,maxGeneration,maxGeneration);
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


bool LossyCompressor::compress(ushort _maxHeight,bool _allowNaturalCompress) {
    allowNaturalCompress=_allowNaturalCompress;
    maxHeight=_maxHeight;
    eliteIdx=65535;
    //std::cerr<<"Genetic algorithm started\n";
    ushort tryTimes=0;
    maxGeneration=200;
    while(tryTimes<3) {
        runGenetic();
        if(getResult().getFitness()<=0) {
            tryTimes++;
            maxGeneration*=2;
        }
        else
            break;
    }
    return getResult().getFitness()>0;
}

const gene& LossyCompressor::getResult() const {
    return population[eliteIdx];
}
