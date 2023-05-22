#include "CopyableTableView.h"
#include <QKeyEvent>
#include <QApplication>
#include <QClipboard>

CopyableTableView::CopyableTableView(QWidget* parent) : QTableView(parent) {}

CopyableTableView::~CopyableTableView() = default;

bool CopyableTableView::event(QEvent* event) noexcept {
  if (event->type() == QEvent::Type::KeyPress) {
    QKeyEvent* const ke = dynamic_cast<QKeyEvent*>(event);
    if (ke != nullptr) {
      if (ke->matches(QKeySequence::StandardKey::Copy)) {
        const auto sel = this->selectedIndexes();

        QString text;
        text.reserve(sel.size() * 64);

        int prev_row = -1;

        for (const auto& qmi : sel) {
          if (!qmi.isValid()) {
            continue;
          }
          const int r = qmi.row();
          const bool is_row_changed = (prev_row != r);
          const bool is_prev_row_valid = (prev_row >= 0);

          prev_row = r;

          if (is_row_changed) {
            if (is_prev_row_valid) {
              text.push_back('\n');
              // another row
            }
            // the first element, do not append any char

          } else {
            text.push_back('\t');  // same row, but different col
          }

          text.append(qmi.data(Qt::ItemDataRole::DisplayRole).toString());
        }

        QApplication::clipboard()->setText(text);

        event->accept();
        emit this->copied();
        return true;
        // do copy here
      }
    }
  }

  return QTableView::event(event);
}