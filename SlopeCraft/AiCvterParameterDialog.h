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

#ifndef AICVTERPARAMETERDIALOG_H
#define AICVTERPARAMETERDIALOG_H

#include <QAbstractButton>
#include <QDialog>
#include <SlopeCraftL.h>

class AiCvterParameterDialog;

namespace Ui {
class AiCvterParameterDialog;
}

class SCWind;

class AiCvterParameterDialog : public QDialog {
  Q_OBJECT

 public:
  explicit AiCvterParameterDialog(SCWind *parent);
  ~AiCvterParameterDialog();

  SlopeCraft::GA_converter_option current_option() const noexcept;

 private slots:
  // void updateMaxFailTimes();
  void on_buttonBox_accepted() noexcept;

  void on_buttonBox_rejected() noexcept;

 private:
  Ui::AiCvterParameterDialog *ui;
};

#endif  // AICVTERPARAMETERDIALOG_H
