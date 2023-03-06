#include "CallbackFunctions.h"
#include <QMessageBox>
#include <sstream>

QWidget *VC_callback::wind{nullptr};

void VC_callback::callback_receive_report(VCL_report_type_t type,
                                          const char *msg,
                                          bool flush) noexcept {
  static std::stringstream ss_warning;

  switch (type) {
  case VCL_report_type_t::information: {
    QMessageBox::information(wind, "Information", QString::fromLocal8Bit(msg));
    return;
  }
  case VCL_report_type_t::warning: {
    ss_warning << msg;

    if (flush) {
      std::string res;
      ss_warning >> res;

      if (res.empty()) {
        return;
      }
      QMessageBox::warning(wind, "Warning", QString::fromLocal8Bit(res));
    }
    return;
  }
  case VCL_report_type_t::error: {
    auto ret = QMessageBox::critical(
        wind, "Fatal error", QString::fromLocal8Bit(msg),
        QMessageBox::StandardButtons{QMessageBox::StandardButton::Close,
                                     QMessageBox::StandardButton::Ignore},
        QMessageBox::StandardButton::Close);

    if (ret == QMessageBox::StandardButton::Close) {
      exit(1919810);
    }
    return;
  }
  }
  abort();
}