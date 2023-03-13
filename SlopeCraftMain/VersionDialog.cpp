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

#include "VersionDialog.h"

VersionDialog::VersionDialog(VersionDialog **_self, QWidget *parent)
    : QDialog(parent), ui(new Ui::VersionDialog) {
  ui->setupUi(this);
  result = userChoice::No;
  self = _self;
}

VersionDialog::~VersionDialog() {
  delete ui;
  *self = nullptr;
  // deleteLater();
}

void VersionDialog::on_BtnYes_clicked() {
  result = userChoice::Yes;
  close();
  // emit finished();
}

void VersionDialog::on_BtnNo_clicked() {
  result = userChoice::No;
  close();
  // emit finished();
}

void VersionDialog::on_BtnNoToAll_clicked() {
  result = userChoice::NoToAll;
  close();
  // emit finished();
}

void VersionDialog::closeEvent(QCloseEvent *event) {
  // result=userChoice::No;
  emit finished();
  QWidget::closeEvent(event);
  deleteLater();
}

VersionDialog::userChoice VersionDialog::getResult() const { return result; }

void VersionDialog::setTexts(const QString &title, const QString &labelText,
                             const QString &browserText) {
  setWindowTitle(title);
  ui->label->setText(labelText);
  ui->textBrowser->setMarkdown(browserText);
}

/*
VersionDialog::userChoice VersionDialog::information(
                                  MainWindow * parent,
                                  const QString & title,
                                  const QString & labelText,
                                  const QString & browserText) {
    VersionDialog form(parent);


    form.show();

    form.setTexts(title,labelText,browserText);

    //connect(parent)

    QEventLoop EL;
    connect(parent,&MainWindow::closed,&form,&VersionDialog::finished);
    connect(&form,&VersionDialog::finished,&EL,&QEventLoop::quit);
    EL.exec();

    VersionDialog::userChoice result=form.result;

    form.close();

    return result;
}

*/
