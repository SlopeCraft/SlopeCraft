#ifndef AICVTERPARAMETERDIALOG_H
#define AICVTERPARAMETERDIALOG_H

#include <QAbstractButton>
#include <QDialog>


namespace Ui {
class AiCvterParameterDialog;
}

class MainWindow;

class AiCvterParameterDialog : public QDialog {
  Q_OBJECT

public:
  explicit AiCvterParameterDialog(AiCvterParameterDialog **, QWidget *parent);
  ~AiCvterParameterDialog();

protected:
  void closeEvent(QCloseEvent *event) override;
private slots:
  void updateMaxFailTimes();
  void on_buttonBox_accepted();

  void on_buttonBox_clicked(QAbstractButton *button);

private:
  Ui::AiCvterParameterDialog *ui;
  AiCvterParameterDialog **self;
};

#endif // AICVTERPARAMETERDIALOG_H
