#include "VersionDialog.h"

VersionDialog::VersionDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VerDialog)
{
    ui->setupUi(this);
    result=userChoice::No;
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


VersionDialog::userChoice VersionDialog::information(QWidget * parent,
                                  const QString & title,
                                  const QString & labelText,
                       const QString & browserText) {
    VersionDialog * form=new VersionDialog(parent);
    form->show();
    form->setWindowTitle(title);
    form->ui->label->setText(labelText);
    form->ui->textBrowser->setText(browserText);

    QEventLoop EL;
    connect(form,&VersionDialog::finished,&EL,&QEventLoop::quit);
    EL.exec();

    VersionDialog::userChoice result=form->result;

    form->close();
    form->deleteLater();

    return result;
}
