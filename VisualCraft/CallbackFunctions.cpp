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

#include "CallbackFunctions.h"
#include <QMessageBox>
#include <sstream>

QWidget *VC_callback::wind{nullptr};

void VC_callback::callback_receive_report(VCL_report_type_t type,
                                          const char *msg,
                                          bool flush) noexcept {
  static std::stringstream ss_warning;

  switch (type) {
  case VCL_report_type_t::information: {
    QMessageBox::information(wind, "Information", QString::fromLocal8Bit(msg));
    return;
  }
  case VCL_report_type_t::warning: {
    ss_warning << msg;

    if (flush) {
      std::string res;
      ss_warning >> res;

      if (res.empty()) {
        return;
      }
      QMessageBox::warning(wind, "Warning", QString::fromLocal8Bit(res));
    }
    return;
  }
  case VCL_report_type_t::error: {
    auto ret = QMessageBox::critical(
        wind, "Fatal error", QString::fromLocal8Bit(msg),
        QMessageBox::StandardButtons{QMessageBox::StandardButton::Close,
                                     QMessageBox::StandardButton::Ignore},
        QMessageBox::StandardButton::Close);

    if (ret == QMessageBox::StandardButton::Close) {
      exit(1919810);
    }
    return;
  }
  }
  abort();
}