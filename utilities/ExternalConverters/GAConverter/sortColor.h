#ifndef SORTCOLOR_H
#define SORTCOLOR_H

#include "GACvterDefines.hpp"

namespace GACvter {

class sortColor
{
public:
    sortColor() noexcept;

    void calculate(ARGB) noexcept;

    inline mapColor_t mapColor(order_t o) const noexcept {
        return mapCs[o];
    }
    /*
    inline float error(order_t o) const {
        return errors[o];
    }
    */

private:
    std::array<mapColor_t,OrderMax> mapCs;
    //std::array<float,OrderMax> errors;
};

}
#endif // SORTCOLOR_H
