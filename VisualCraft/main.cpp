#include <QApplication>

#include "CallbackFunctions.h"
#include "VCWind.h"
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QTranslator>

QNetworkAccessManager *global_manager{nullptr};

bool parse_config_json(QString &err) noexcept;

int main(int argc, char **argv) {
  QApplication qapp(argc, argv);
  QDir::setCurrent(QCoreApplication::applicationDirPath());
  QTranslator translator;

  ::is_language_ZH = QLocale::system().uiLanguages().contains("zh");

  // this line is used to test the translation
  //::is_language_ZH = false;

  if (!::is_language_ZH) {
    if (translator.load(":/i18n/VisualCraft_en_US.qm")) {
      qapp.installTranslator(&translator);
    }
  }

  if (!VCL_is_version_ok()) {
    QMessageBox::critical(
        nullptr, VCWind::tr("VisualCraftL动态库版本不匹配"),
        VCWind::tr(
            "界面程序编译时使用的VisualCraftL版本为%"
            "1，而VisualCraftL动态库的版本为%2。通常这是因为动态库版本过低。")
            .arg(SC_VERSION_STR)
            .arg(VCL_version_string()));
    qapp.exit(1);
    return 1;
  }

  {
    QString err;
    if (!parse_config_json(err)) {
      QMessageBox::critical(nullptr, VCWind::tr("加载配置文件失败。"), err);
      qapp.exit(1);
      return 1;
    }
  }

  QNetworkAccessManager manager;
  global_manager = &manager;

  VCWind wind;

  VC_callback::wind = &wind;

  VCL_set_report_callback(VC_callback::callback_receive_report);

  wind.show();

  wind.retrieve_latest_version(
      "https://api.github.com/repos/ToKiNoBug/SlopeCraft/releases/latest",
      manager, "https://github.com/ToKiNoBug/SlopeCraft/releases/latest",
      false);

  return qapp.exec();
}

#include <fstream>
#include <json.hpp>

bool parse_config_json(QString &err) noexcept {
  err = "";
  using njson = nlohmann::json;
  njson jo;

  try {
    std::fstream file{"./vc-config.json"};
    jo = njson::parse(file, nullptr, true, true);

    VCWind::default_json = jo.at("default_block_state_list");
    VCWind::default_zip_12 = jo.at("default_resource_pack_12");
    VCWind::default_zip_latest = jo.at("default_resource_pack_latest");

  } catch (std::exception &e) {
    err =
        VCWind::tr("无法加载配置文件\"./vc-config.json\"。\n%1").arg(e.what());
    return false;
  }

  return true;
}