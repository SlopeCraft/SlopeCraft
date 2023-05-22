#include <QApplication>
#include "SCWind.h"
#include "VersionDialog.h"
#include <QDir>

int main(int argc, char** argv) {
  QApplication qapp(argc, argv);

  SCWind wind;

  wind.show();

  QDir::setCurrent(QCoreApplication::applicationDirPath());

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