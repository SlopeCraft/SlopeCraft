#ifndef VERSIONDIALOG_H
#define VERSIONDIALOG_H

#include <QWidget>
#include <QEvent>
#include <QEventLoop>
#include "ui_VersionDialog.h"

namespace Ui {
class VersionDialog;
}

class QWidget;

class VersionDialog : public QWidget
{
    Q_OBJECT
public:
    explicit VersionDialog(QWidget *parent = nullptr);
    ~VersionDialog();
    enum userChoice {
        Yes,
        No,
        NoToAll
    };

    static userChoice information(QWidget * parent,
                                  const QString & title,
                                  const QString & labelText,
                                  const QString & browserText);
signals:
    void finished();

private slots:
    void on_BtnYes_clicked();

    void on_BtnNo_clicked();

    void on_BtnNoToAll_clicked();

protected:

private:
    Ui::VerDialog * ui;
    userChoice result;

};

#endif // VERSIONDIALOG_H
