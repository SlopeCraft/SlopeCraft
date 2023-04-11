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
#include <QDir>

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

void VCWind::on_ac_tutorial_triggered() noexcept {
  QDesktopServices::openUrl(QUrl{"https://slopecraft.readthedocs.io/"});
}

void VCWind::on_ac_about_VisualCraft_triggered() noexcept {
  QMessageBox::information(
      this, VCWind::tr("关于 VisualCraft"),
      VCWind::tr(
          "VisualCraft 是一款全新的 Minecraft 像素画生"
          "成器，由 MC 玩家 TokiNoBug 开发，是 SlopeCraft 的子项目。与其他类似"
          "的软件不同，VisualCraft 旨在跟进最新的 MC 版本 (1.12~最新版)"
          "、支持最多的 MC 特性"
          "，提供最强大的功能。\n\n") +
          VCWind::tr(
              "目前 VisualCraft 能够解析许多第三方资源包，也允许自定义增加"
              "加新方块。与传统的思路不同，VisualCraft 以方块模型的方式来"
              "解析资源包，尽量贴近 Minecraft 的方式，因此支持各种自定义的"
              "方块模型。\n\n") +
          VCWind::tr("在导出方面，VisualCraft 支持 Litematica "
                     "mod 的投影 (*.litematic)、WorldEdit "
                     "原理图 (*.shem)(仅 1.13+可用)、原版结构方块文件 (*.nbt)"
                     "、平面示意图 (*.png) 等方式。\n\n") +
          VCWind::tr(
              "VisualCraft 支持用各种透明方块互相叠加，产生更多的颜色。软"
              "件最多支持不超过 65534 种颜色，受此限制，像素画的层数不超过"
              "3 层。\n\n") +
          (VCWind::tr(
               "由于颜色数量很多，VisualCraft 使用了显卡加速。目前支持的 AP"
               "I 有 OpenCL。现在正在使用的 API 是%1")
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
  VersionDialog::start_network_request(this, "VisualCraft", url_api, manager,
                                       is_manually);
}

#include "CallbackFunctions.h"

void VCWind::on_ac_flush_warnings_triggered() noexcept {
  VC_callback::callback_receive_report(VCL_report_type_t::warning, "Flush\n",
                                       true);
}