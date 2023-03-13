/*
 Copyright © 2021-2023  TokiNoBug
This file is part of SlopeCraft.

    SlopeCraft is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SlopeCraft is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SlopeCraft. If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/SlopeCraft/SlopeCraft
    bilibili:https://space.bilibili.com/351429231
*/

#include "BiomeBrowser.h"
#include "BlockBrowser.h"
#include "BlockSelector.h"
#include "ColorBrowser.h"
#include "VCWind.h"
#include "ui_VCWind.h"
#include <QDesktopServices>
#include <QMessageBox>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <VersionDialog.h>

void VCWind::on_ac_browse_block_triggered() noexcept {
  BlockBrowser *bb = new BlockBrowser(this);

  bb->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose, true);
  bb->setAttribute(Qt::WidgetAttribute::WA_AlwaysStackOnTop, true);
  // bb->setAttribute(Qt::WidgetAttribute::WA_NativeWindow, true);
  bb->setWindowFlag(Qt::WindowType::Window, true);

  connect(this, &VCWind::signal_basic_colorset_changed, bb,
          &QWidget::deleteLater);

  bb->show();
}

void VCWind::on_ac_about_VisualCraft_triggered() noexcept {
  QMessageBox::information(
      this, VCWind::tr("关于VisualCraft"),
      VCWind::tr("VisualCraft是一款全新的Minecraft像素画生"
                 "成器，由MC玩家TokiNoBug开发，是SlopeCraft的子项目。与其他类似"
                 "的软件不同，VisualCraft旨在跟进最新的MC版本(1.12~最新版)"
                 "、支持最多的MC特性"
                 "，提供最强大的功能。\n\n") +
          VCWind::tr("目前VisualCraft能够解析许多第三方资源包，也允许自定义增加"
                     "加新方块。与传统的思路不同，VisualCraft以方块模型的方式来"
                     "解析资源包，尽量贴近Minecraft的方式，因此支持各种自定义的"
                     "方块模型。\n\n") +
          VCWind::tr("在导出方面，VisualCraft支持Litematica "
                     "mod的投影(*.litematic)、WorldEdit "
                     "原理图(*.shem)(仅1.13+可用)、原版结构方块文件(*.nbt)"
                     "、平面示意图(*.png)等方式。\n\n") +
          VCWind::tr("VisualCraft支持用各种透明方块互相叠加，产生更多的颜色。软"
                     "件最多支持不超过65534种颜色，受此限制，像素画的层数不超过"
                     "3层。\n\n") +
          (VCWind::tr(
               "由于颜色数量很多，VisualCraft使用了显卡加速。目前支持的AP"
               "I有OpenCL。现在正在使用的API是%1")
               .arg(VCL_get_GPU_api_name())));
}

void VCWind::on_ac_browse_biome_triggered() noexcept {
  BiomeBrowser *bb = new BiomeBrowser(this);

  bb->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose, true);
  bb->setAttribute(Qt::WidgetAttribute::WA_AlwaysStackOnTop, true);
  // bb->setAttribute(Qt::WidgetAttribute::WA_NativeWindow, true);
  bb->setWindowFlag(Qt::WindowType::Window, true);

  connect(this, &VCWind::signal_basic_colorset_changed, bb,
          &QWidget::deleteLater);

  bb->show();
}

void VCWind::on_ac_contact_bilibili_triggered() noexcept {
  QDesktopServices::openUrl(QUrl("https://space.bilibili.com/351429231"));
}
void VCWind::on_ac_contact_github_repo_triggered() noexcept {
  QDesktopServices::openUrl(QUrl("https://github.com/SlopeCraft"));
}
void VCWind::on_ac_report_bugs_triggered() noexcept {
  QDesktopServices::openUrl(
      QUrl("https://github.com/SlopeCraft/SlopeCraft/issues/new/choose"));
}

void VCWind::on_pb_custom_select_clicked() noexcept {

  BlockSelector *bs = new BlockSelector(this);

  bs->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose, true);
  bs->setAttribute(Qt::WidgetAttribute::WA_AlwaysStackOnTop, true);
  // bb->setAttribute(Qt::WidgetAttribute::WA_NativeWindow, true);
  bs->setWindowFlag(Qt::WindowType::Window, true);

  bs->show();
}

void VCWind::on_ac_browse_basic_colors_triggered() noexcept {
  ColorBrowser *cb = new ColorBrowser(this);

  cb->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose, true);
  cb->setAttribute(Qt::WidgetAttribute::WA_AlwaysStackOnTop, true);
  // bb->setAttribute(Qt::WidgetAttribute::WA_NativeWindow, true);
  cb->setWindowFlag(Qt::WindowType::Window, true);

  connect(this, &VCWind::signal_basic_colorset_changed, cb,
          &QWidget::deleteLater);

  cb->show();

  cb->setup_table_basic();
}

void VCWind::on_ac_browse_allowed_colors_triggered() noexcept {

  ColorBrowser *cb = new ColorBrowser(this);

  cb->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose, true);
  cb->setAttribute(Qt::WidgetAttribute::WA_AlwaysStackOnTop, true);
  // bb->setAttribute(Qt::WidgetAttribute::WA_NativeWindow, true);
  cb->setWindowFlag(Qt::WindowType::Window, true);

  connect(this, &VCWind::signal_allowed_colorset_changed, cb,
          &QWidget::deleteLater);

  cb->show();

  cb->setup_table_allowed();
}

void VCWind::on_ac_check_update_triggered() noexcept {
  this->retrieve_latest_version(::url_for_update, *global_manager, true);
}

void VCWind::retrieve_latest_version(QString url_api,
                                     QNetworkAccessManager &manager,
                                     bool is_manually) noexcept {
  QNetworkRequest request(url_api);

  QNetworkReply *reply = manager.get(request);

  // connect(reply, &QNetworkReply::finished, this,
  //         &VCWind::when_network_finished);
  connect(reply, &QNetworkReply::finished, [this, reply, is_manually]() {
    this->when_network_finished(reply, is_manually);
  });
}

void VCWind::when_network_finished(QNetworkReply *reply,
                                   bool is_manually) noexcept {

  const QByteArray content_qba = reply->readAll();
  version_info info;
  try {
    info = extract_latest_version(content_qba.data());
  } catch (std::exception &e) {
    QMessageBox::warning(
        this, VCWind::tr("获取最新版本失败"),
        VCWind::tr(
            "解析 \"%1\" "
            "返回的结果时出现错误：\n\n%"
            "2\n\n这不是一个致命错误，不影响软件使用。\n解析失败的信息为：\n%3")
            .arg(reply->url().toString())
            .arg(e.what())
            .arg(content_qba),
        QMessageBox::StandardButtons{QMessageBox::StandardButton::Ignore});
    reply->deleteLater();
    return;
  }

  reply->deleteLater();

  const uint64_t latest_ver = info.version_u64;

  const auto &tag_name = info.tag_name;

  const uint64_t ui_ver = SC_VERSION_U64;

  if (ui_ver == latest_ver) {

    if (is_manually) {
      QMessageBox::information(this, VCWind::tr("检查更新成功"),
                               VCWind::tr("您在使用的是最新版本"));
    }

    return;
  }

  if (ui_ver > latest_ver) {
    if (is_manually) {
      QMessageBox::information(
          this, VCWind::tr("检查更新成功"),
          VCWind::tr("您使用的版本(%1)比已发布的(%2)更新，可能是测试版。")
              .arg(SC_VERSION_STR)
              .arg(tag_name));
    }
    return;
  }

  {
    VersionDialog *vd = new VersionDialog(this);
    vd->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose, true);
    vd->setAttribute(Qt::WidgetAttribute::WA_AlwaysStackOnTop, true);
    vd->setWindowFlag(Qt::WindowType::Window, true);

    vd->setup_text(VCWind::tr("VisualCraft已更新"),
                   VCWind::tr("最新版本为%1，当前版本为%2（内核版本%3）")
                       .arg(tag_name)
                       .arg(SC_VERSION_STR)
                       .arg(VCL_version_string()),
                   info.body, info.html_url);

    vd->show();
    return;
  }
}