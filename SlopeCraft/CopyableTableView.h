#ifndef SLOPECRAFT_SLOPECRAFT_COPYABLETABLEVIEW_H
#define SLOPECRAFT_SLOPECRAFT_COPYABLETABLEVIEW_H

#include <QTableView>
#include <QEvent>

class CopyableTableView : public QTableView {
  Q_OBJECT
 private:
 public:
  explicit CopyableTableView(QWidget* parent = nullptr);
  ~CopyableTableView();

 protected:
  bool event(QEvent* event) noexcept override;

 signals:
  void copied();
};

#endif  // SLOPECRAFT_SLOPECRAFT_COPYABLETABLEVIEW_H