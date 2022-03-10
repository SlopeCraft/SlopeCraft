#include "AiCvterParameterDialog.h"
#include "ui_AiCvterParameterDialog.h"

AiCvterParameterDialog::AiCvterParameterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AiCvterParameterDialog) {
    ui->setupUi(this);
}

AiCvterParameterDialog::~AiCvterParameterDialog() {
    delete ui;
}
