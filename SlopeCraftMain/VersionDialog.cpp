/*
 Copyright © 2021  TokiNoBug
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
    along with SlopeCraft.  If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/ToKiNoBug
    bilibili:https://space.bilibili.com/351429231
*/

#include "VersionDialog.h"

VersionDialog::VersionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VersionDialog)
{
    ui->setupUi(this);
    result=userChoice::No;
    this->setAttribute(Qt::WA_QuitOnClose,false);
}

VersionDialog::~VersionDialog() {
    delete  ui;
}

void VersionDialog::on_BtnYes_clicked() {
    result=userChoice::Yes;
    emit finished();
}

void VersionDialog::on_BtnNo_clicked() {
    result=userChoice::No;
    emit finished();
}

void VersionDialog::on_BtnNoToAll_clicked() {
    result=userChoice::NoToAll;
    emit finished();
}

void VersionDialog::closeEvent(QCloseEvent * event) {
    emit finished();
    QWidget::closeEvent(event);
}

VersionDialog::userChoice VersionDialog::information(QWidget * parent,
                                  const QString & title,
                                  const QString & labelText,
                       const QString & browserText) {
    VersionDialog * form=new VersionDialog(parent);

    //QObject::connect(parent,&QWidget::destroyed,window,&VersionDialog::deleteLater);

    form->show();

    form->setWindowTitle(title);
    form->ui->label->setText(labelText);
    form->ui->textBrowser->setMarkdown(browserText);

    //connect(parent)

    QEventLoop EL;
    connect(form,&VersionDialog::finished,&EL,&QEventLoop::quit);
    EL.exec();

    VersionDialog::userChoice result=form->result;

    form->deleteLater();

    return result;
}
