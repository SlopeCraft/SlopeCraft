#include <format>
#include <cstddef>
#include <cstdlib>
#include <string_view>
#include <QApplication>
#include <QImageReader>
#include <QDir>
#include <QCommandLineParser>
#include "SCWind.h"
#include "VersionDialog.h"

#include <iostream>

int main(int argc, char** argv) {
  QApplication qapp(argc, argv);
  QCommandLineParser parser;
  parser.setApplicationDescription("Minecraft map art generator");

  parser.addOption(QCommandLineOption{
    "build-dir-mode",
    "Read blocks from local dir. Only useful for development"});
  parser.addOption(
      QCommandLineOption{"lang", "Force UI language", "<zh|en>", "zh"});
  parser.process(qapp);

  QDir::setCurrent(QCoreApplication::applicationDirPath());
  QImageReader::setAllocationLimit(INT32_MAX);

  const auto config = [&]() {
    const QString SC_default_blocks_dir =
        QStringLiteral(SC_RESOURCE_DIR "/Blocks_SCL");
    app_config cfg;
    if (parser.isSet("build-dir-mode")) {
      cfg.blocks_dir_path = "../SCL_block_lists";
    } else {
      cfg.blocks_dir_path = SC_default_blocks_dir;
    }
    return cfg;
  }();

  SCWind wind{nullptr, config};

  wind.show();
  wind.setWindowTitle(SCWind::default_wind_title());

  bool is_language_ZH = QLocale::system().uiLanguages().contains("zh");

  // this line is used to test the translation
  if (parser.isSet("lang")) {
    const QString lang = parser.value("lang");
    if (lang == "zh") {
      is_language_ZH = true;
    } else if (lang == "en") {
      is_language_ZH = false;
    } else {
      std::cerr << "Unsupported language '" << lang.toStdString() << "'"
                << std::endl;
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