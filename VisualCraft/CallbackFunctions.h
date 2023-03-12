/*
 Copyright © 2021-2023  TokiNoBug
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
    along with SlopeCraft. If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/SlopeCraft/SlopeCraft
    bilibili:https://space.bilibili.com/351429231
*/

#ifndef SLOPECRAFT_VISUALCRAFT_CALLBACKFUNCTIONS_H
#define SLOPECRAFT_VISUALCRAFT_CALLBACKFUNCTIONS_H

#include <VisualCraftL.h>

class QWidget;
namespace VC_callback {
void callback_receive_report(VCL_report_type_t, const char *msg,
                             bool flush) noexcept;

extern QWidget *wind;
} // namespace VC_callback

#endif // SLOPECRAFT_VISUALCRAFT_CALLBACKFUNCTIONS_H