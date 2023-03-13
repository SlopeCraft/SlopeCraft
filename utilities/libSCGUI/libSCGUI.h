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

#ifndef SLOPECRAFT_UTILITIES_LIBSCGUI_LIBSCGUI_H
#define SLOPECRAFT_UTILITIES_LIBSCGUI_LIBSCGUI_H

#include <libAbstractGUI.h>

#include <SlopeCraftL.h>

namespace libSCGUI {
class SCGUI {
private:
  ::SlopeCraft::Kernel *kernel;
  ::std::string prev_dir = "";

public:
  libAbstractGUI::abstract_widget main_window;
  libAbstractGUI::label label_show_raw_image;
  // these 2 labels are about image conversion, actually they can be equal.
  libAbstractGUI::label label_show_image_before_convert;
  libAbstractGUI::label label_show_image_after_convert;

  libAbstractGUI::progress_bar progressbar_convert_image;

public:
  SCGUI() { kernel = ::SlopeCraft::Kernel::create(); }

public:
  void on_load_image_clicked() noexcept;
  void on_convert_clicked(const ::SCL_convertAlgo, const bool dither) noexcept;
  void refreash_raw_image() noexcept;
  void refreash_converted_image() noexcept;

  void on_build3D_clicked() noexcept;
  void on_export3D_clicked() noexcept;
};
} // namespace libSCGUI

#endif // SLOPECRAFT_UTILITIES_LIBSCGUI_LIBSCGUI_H