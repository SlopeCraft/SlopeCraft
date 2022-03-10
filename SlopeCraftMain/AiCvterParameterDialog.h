#ifndef AICVTERPARAMETERDIALOG_H
#define AICVTERPARAMETERDIALOG_H

#include <QDialog>

namespace Ui {
class AiCvterParameterDialog;
}

class AiCvterParameterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AiCvterParameterDialog(QWidget *parent = nullptr);
    ~AiCvterParameterDialog();

private:
    Ui::AiCvterParameterDialog *ui;
};

#endif // AICVTERPARAMETERDIALOG_H
