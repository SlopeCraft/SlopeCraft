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

#ifndef VERSIONDIALOG_H
#define VERSIONDIALOG_H

#include <QCloseEvent>
#include <QDialog>
#include <QEvent>
#include <QEventLoop>

namespace Ui {
class VersionDialog;
}

class QWidget;

class VersionDialog : public QDialog {
  Q_OBJECT
public:
  explicit VersionDialog(QWidget *parent = nullptr);
  ~VersionDialog();
  enum userChoice { Yes, No, NoToAll };

  void setTexts(const QString &title, const QString &labelText,
                const QString &browserText);
  userChoice getResult() const;
  /*
  static userChoice information(MainWindow * parent,
                                const QString & title,
                                const QString & labelText,
                                const QString & browserText);
  */
signals:
  void finished();

private slots:
  void on_BtnYes_clicked();

  void on_BtnNo_clicked();

  void on_BtnNoToAll_clicked();

protected:
  void closeEvent(QCloseEvent *) override;

private:
  Ui::VersionDialog *ui;
  userChoice result;
};

#endif // VERSIONDIALOG_H
