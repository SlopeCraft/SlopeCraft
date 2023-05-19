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

#include "AiCvterParameterDialog.h"
#include "ui_AiCvterParameterDialog.h"

#include "SCWind.h"

using namespace SlopeCraft;
AiCvterParameterDialog::AiCvterParameterDialog(SCWind *parent,
                                               SlopeCraft::Kernel *kernelp)
    : QDialog(parent), ui(new Ui::AiCvterParameterDialog), kernel(kernelp) {
  this->ui->setupUi(this);

  SlopeCraft::AiCvterOpt opt = *this->kernel->aiCvterOpt();
  opt.version = SC_VERSION_U64;

  this->ui->sb_pop_size->setValue(opt.popSize);
  this->ui->sb_max_gen->setValue(opt.maxGeneration);
  this->ui->sb_max_early_stop->setValue(opt.maxFailTimes);
  this->ui->dsb_crossover_prob->setValue(opt.crossoverProb);
  this->ui->dsb_mutate_prob->setValue(opt.mutationProb);
}

AiCvterParameterDialog::~AiCvterParameterDialog() { delete this->ui; }

SlopeCraft::AiCvterOpt AiCvterParameterDialog::current_option() const noexcept {
  SlopeCraft::AiCvterOpt ret;
  ret.crossoverProb = this->ui->dsb_crossover_prob->value();
  ret.mutationProb = this->ui->dsb_mutate_prob->value();
  ret.maxFailTimes = this->ui->sb_max_early_stop->value();
  ret.maxGeneration = this->ui->sb_max_gen->value();
  ret.popSize = this->ui->sb_pop_size->value();
  return ret;
}

void AiCvterParameterDialog::on_buttonBox_accepted() noexcept {
  auto opt = this->current_option();
  this->kernel->setAiCvterOpt(&opt);
}

void AiCvterParameterDialog::on_buttonBox_rejected() noexcept {
  this->deleteLater();
}
