#include "newTokiColor.hpp"

std::array<uint8_t, 4> newTokiColorBase<true>::DepthCount = {64, 64, 64, 64};
bool newTokiColorBase<true>::needFindSide = false;
/*
SCL_convertAlgo newTokiColorBase<true>::convertAlgo =
    SCL_convertAlgo::RGB_Better;
    */
