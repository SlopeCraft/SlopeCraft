//
// Created by joseph on 4/12/24.
//

#include "Colorset.h"
#include "SCLDefines.h"

const std::unique_ptr<const colorset_basic_t> color_set::basic{
    new colorset_basic_t{RGBBasicSource}};