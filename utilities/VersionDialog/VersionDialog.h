#ifndef SLOPECRAFT_VISUALCRAFT_VERSION_DIALOG_H
#define SLOPECRAFT_VISUALCRAFT_VERSION_DIALOG_H

#include <QDialog>
#include <SC_version_buildtime.h>

class VersionDialog;

namespace Ui {
class VersionDialog;
}

class VersionDialog : public QDialog {
  Q_OBJECT
private:
  Ui::VersionDialog *ui;
  QString url_download{""};

public:
  explicit VersionDialog(QWidget *parent);
  ~VersionDialog();

  void setup_text(QString title, QString content, QString markdown_content,
                  QString url_download) noexcept;
private slots:
  void on_pb_download_clicked() noexcept;
};

struct version_info {
  QString tag_name;
  QString body;
  QString html_url;
  uint64_t version_u64;
};

version_info
extract_latest_version(std::string_view json_all_releaese) noexcept(false);

#endif // SLOPECRAFT_VISUALCRAFT_VERSION_DIALOG_H