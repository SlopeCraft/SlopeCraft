#ifndef GACONVERTER_H
#define GACONVERTER_H

#include "GACvterDefines.hpp"
#include "sortColor.h"
#include "uiPack.h"

namespace GACvter {

void iFun(Var_t *, const CvterInfo *) noexcept;
void fFun(const Var_t *, const CvterInfo *, double *) noexcept;
void cFun(const Var_t *, const Var_t *, Var_t *, Var_t *, const CvterInfo *) noexcept;
void mFun(const Var_t *,Var_t *, const CvterInfo *) noexcept;

void updateMapColor2GrayLUT();

class GAConverter : private heu::SOGA<Var_t,
        heu::FitnessOption::FITNESS_LESS_BETTER,
        heu::RecordOption::DONT_RECORD_FITNESS,
        CvterInfo,
        iFun, fFun, cFun, mFun>
{
    using Base_t = heu::SOGA<Var_t,
    FitnessOption::FITNESS_LESS_BETTER,
    RecordOption::DONT_RECORD_FITNESS,
    CvterInfo,
    ::GACvter::iFun,::GACvter::fFun,::GACvter::cFun,::GACvter::mFun>;
public:
    GAConverter();

    void setUiPack(const uiPack &) noexcept;

    inline void setUiPtr(void * _uiptr) noexcept {
        this->_args.ui._uiPtr=_uiptr;
    }

    inline void setProgressRangeFun(void(*fPtr)(void*,int,int,int)) noexcept {
        this->_args.ui.progressRangeSet=fPtr;
    }

    inline void setProgressAddFun(void(*fPtr)(void*,int)) noexcept {
        this->_args.ui.progressAdd=fPtr;
    }

    void setRawImage(const EImage &) noexcept;
    void setSeeds(const std::vector<const Eigen::ArrayXX<mapColor_t> *> &) noexcept;

    using Base_t::setOption;
    using Base_t::option;

    void run();

    void resultImage(EImage *);

private:
    friend void ::GACvter::iFun(Var_t *, const CvterInfo *) noexcept;
    friend void ::GACvter::fFun(const Var_t *, const CvterInfo *, double *) noexcept;
    friend void ::GACvter::cFun(const Var_t *, const Var_t *, Var_t *, Var_t *, const CvterInfo *) noexcept;
    friend void ::GACvter::mFun(const Var_t *,Var_t *, const CvterInfo *) noexcept;

    HEU_MAKE_GABASE_TYPES(Base_t)
    friend class heu::internal::GABase<Var_t,double,
    RecordOption::DONT_RECORD_FITNESS,Gene_t,CvterInfo,
        ::GACvter::iFun,::GACvter::fFun,::GACvter::cFun,::GACvter::mFun>;


    static constexpr std::clock_t reportInterval = 2*CLOCKS_PER_SEC;
protected:
    template<class this_t=GAConverter>
    inline void __impl_recordFitness() noexcept {
        Base_t::template __impl_recordFitness<this_t>();

        this->_args.strongMutation =generation()*2< option().maxGenerations;
        if(generation() % 10==0) {
            std::clock_t curT=std::clock();
            if(curT-args().prevClock >= reportInterval) {
                args().ui.rangeSet(0,option().maxGenerations,generation());
                this->_args.prevClock=curT;
            }
        }
    }

};

}//namespace GACvter

#endif // GACONVERTER_H
