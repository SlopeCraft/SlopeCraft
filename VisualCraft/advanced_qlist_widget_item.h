#ifndef SLOPECRAFT_VISUALCRAFT_advanced_qlist_widget_item_hpp
#define SLOPECRAFT_VISUALCRAFT_advanced_qlist_widget_item_hpp

#include <QApplication>
#include <QListWidgetItem>

class advanced_qlwi : public QListWidgetItem {
  // Q_OBJECT
private:
  const bool _is_special{false};

public:
  advanced_qlwi(QString text, bool is_special)
      : QListWidgetItem(text), _is_special(is_special) {
    this->setIcon(QIcon(QApplication::style()->standardIcon(
        QStyle::StandardPixmap::SP_FileIcon)));
    this->setCheckState(Qt::Checked);
  }

  bool is_special() const noexcept { return this->_is_special; }

  // virtual
  //~advanced_qlwi() = default;
};

#endif // SLOPECRAFT_VISUALCRAFT_advanced_qlist_widget_item_hpp