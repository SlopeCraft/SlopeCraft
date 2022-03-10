#include "AiCvterParameterDialog.h"
#include "ui_AiCvterParameterDialog.h"

#include "MainWindow.h"

using namespace SlopeCraft;
AiCvterParameterDialog::AiCvterParameterDialog(AiCvterParameterDialog ** _self,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AiCvterParameterDialog) {
    ui->setupUi(this);

    MainWindow * wind=qobject_cast<MainWindow *>(parent);
    Kernel * k=wind->kernelPtr();
    const AiCvterOpt * src=k->aiCvterOpt();

    ui->crossoverProb->setValue(SCL_getCrossoverProb(src));
    ui->mutateProb->setValue(SCL_getMutationProb(src));
    ui->maxGeneration->setValue(SCL_getMaxGeneration(src));
    ui->popSize->setValue(SCL_getPopSize(src));

    connect(ui->maxGeneration,&QSpinBox::valueChanged,
            this,&AiCvterParameterDialog::updateMaxFailTimes);
    connect(ui->maxFailTimes,&QSpinBox::valueChanged,
            this,&AiCvterParameterDialog::updateMaxFailTimes);
    connect(ui->enableFailTimes,&QCheckBox::clicked,
            this,&AiCvterParameterDialog::updateMaxFailTimes);

    updateMaxFailTimes();
    ui->maxFailTimes->setValue(SCL_getMaxFailTimes(src));

    ui->enableFailTimes->setChecked(SCL_getMaxFailTimes(src)<SCL_getMaxGeneration(src));

    self=_self;
    *self=this;
}

AiCvterParameterDialog::~AiCvterParameterDialog() {
    delete ui;
    *self=nullptr;
}

void AiCvterParameterDialog::updateMaxFailTimes() {
    ui->maxFailTimes->setMaximum(ui->maxGeneration->value());

    ui->maxFailTimes->setEnabled(ui->enableFailTimes->isChecked());

    if(!ui->enableFailTimes->isChecked()) {
        ui->maxFailTimes->setValue(ui->maxGeneration->value());
    }
}

void AiCvterParameterDialog::closeEvent(QCloseEvent *event) {
    QDialog::closeEvent(event);

    deleteLater();
}

void AiCvterParameterDialog::on_buttonBox_accepted() {
    AiCvterOpt * opt=SCL_createAiCvterOpt();
    SCL_setCrossoverProb(opt,ui->crossoverProb->value());
    SCL_setMaxFailTimes(opt,ui->maxFailTimes->value());
    SCL_setMaxGeneration(opt,ui->maxGeneration->value());
    SCL_setMutationProb(opt,ui->mutateProb->value());
    SCL_setPopSize(opt,ui->popSize->value());

    MainWindow * wind=qobject_cast<MainWindow *>(parent());
    Kernel * k=wind->kernelPtr();

    k->setAiCvterOpt(opt);
}


void AiCvterParameterDialog::on_buttonBox_clicked(QAbstractButton *) {
    deleteLater();
}

