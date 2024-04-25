#include <fmt/format.h>
#include <iostream>
#include <QApplication>
#include <QDir>
#include "SCWind.h"
#include "VersionDialog.h"

int main(int argc, char** argv) {
  QApplication qapp(argc, argv);

  QDir::setCurrent(QCoreApplication::applicationDirPath());

  SCWind wind;

  wind.show();
  wind.setWindowTitle(SCWind::default_wind_title());

  bool is_language_ZH = QLocale::system().uiLanguages().contains("zh");

  // this line is used to test the translation

  for (int i = 0; i < argc; i++) {
    if (std::string_view(argv[i]) == "--lang-force-to-en") {
      is_language_ZH = false;
      break;
    }
    if (std::string_view(argv[i]) == "--lang-force-to-zh") {
      is_language_ZH = true;
      break;
    }
  }

  if (is_language_ZH) {
    wind.set_lang(::SCL_language::Chinese);
  } else {
    wind.set_lang(::SCL_language::English);
  }

  VersionDialog::start_network_request(&wind, "SlopeCraft",
                                       QUrl{SCWind::update_url},
                                       SCWind::network_manager(), false);

  return qapp.exec();
}