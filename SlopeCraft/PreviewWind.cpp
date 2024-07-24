#include <QString>
#include <QModelIndex>
#include <QFileDialog>
#include <QMessageBox>
#include <algorithm>
#include "PreviewWind.h"
#include "ui_PreviewWind.h"
#include "SCWind.h"
#include "CopyableTableView.h"

PreviewWind::PreviewWind(QWidget* parent)
    : QDialog(parent), ui(new Ui::PreviewWind) {
  this->ui->setupUi(this);

  this->mmp = new MaterialModel(this);
  this->mmp->set_mat_list_pointer(&this->mat_list);
  // this->ui->tv_mat->setModel(this->mmp);

  // connect(this->mmp,&QAbstractTableModel::dataChanged,this->ui->tv_mat,&QTableView::)

  connect(this->ui->cb_show_in_stacks, &QCheckBox::toggled, this->mmp,
          &MaterialModel::refresh);
  connect(this->ui->cb_sort_option, &QComboBox::currentIndexChanged, this->mmp,
          &MaterialModel::refresh);
  this->ui->tv_mat->setModel(this->mmp);

  auto on_copied = [this]() noexcept {
    this->setWindowTitle(
        tr("%1 -- 表格内容已复制到剪贴板").arg(tr("查看材料列表")));
  };
  connect(this->ui->tv_mat, &CopyableTableView::copied, on_copied);
  connect(this->ui->pb_copy_to_clipboard, &QPushButton::clicked,
          this->ui->tv_mat, &CopyableTableView::copied);
}

PreviewWind::~PreviewWind() {}

void PreviewWind::set_size(std::span<size_t, 3> size) & noexcept {
  QString size_str =
      tr("大小：%1 × %2 × %3").arg(size[0]).arg(size[1]).arg(size[2]);

  this->ui->lb_show_size->setText(size_str);

  this->ui->lb_show_volume->setText(
      tr("体积：%1").arg(size[0] * size[1] * size[2]));
}

void PreviewWind::set_total_count(size_t count) & noexcept {
  this->ui->lb_block_count->setText(tr("方块总数：%1").arg(count));
}

void PreviewWind::setup_data(
    const SlopeCraft::color_table& table,
    const SlopeCraft::structure_3D& structure) noexcept {
  std::array<size_t, 64> count_list;
  count_list.fill(0);
  table.stat_blocks(structure, count_list.data());
  {
    size_t total_blocks{0};
    for (auto c : count_list) {
      total_blocks += c;
    }
    this->set_total_count(total_blocks);
  }

  std::vector<const SlopeCraft::mc_block_interface*> blkp_arr;
  table.visit_blocks([&blkp_arr](const SlopeCraft::mc_block_interface* b) {
    blkp_arr.emplace_back(b);
  });

  this->mat_list.reserve(blkp_arr.size());
  for (size_t idx = 0; idx < blkp_arr.size(); idx++) {
    if (count_list[idx] > 0) {
      this->mat_list.emplace_back(
          material_item{.blk = blkp_arr[idx], .count = count_list[idx]});
    }
  }

  {
    std::array<size_t, 3> sz{structure.shape_x(), structure.shape_y(),
                             structure.shape_z()};
    this->set_size(sz);
  }

  // this->ui->tv_mat->setModel(this->mmp);
  emit this->mmp->layoutChanged();
  // this->mmp->refresh();
  // this->ui->tv_mat->doItemsLayout();
}

bool PreviewWind::is_unit_stack() const noexcept {
  return this->ui->cb_show_in_stacks->isChecked();
}

PreviewWind::sort_option PreviewWind::current_sort_option() const noexcept {
  const int cidx = this->ui->cb_sort_option->currentIndex();
  switch (cidx) {
    case 1:
      return sort_option::ascending;
    case 2:
      return sort_option::descending;
    default:
      return sort_option::no_sort;
  }
}

void PreviewWind::on_pb_export_file_clicked() noexcept {
  const QString out_file = QFileDialog::getSaveFileName(
      this, tr("保存材料表"), this->export_mat_list_prev_dir, "*.csv");
  if (out_file.isEmpty()) {
    return;
  }
  this->export_mat_list_prev_dir = QFileInfo{out_file}.dir().path();

  QFile ofile{out_file, this};
  if (not ofile.open(QIODevice::OpenMode::enum_type::WriteOnly)) {
    QMessageBox::warning(this, tr("保存材料表失败"),
                         tr("无法打开文件 \"%1\"，详细信息：%2")
                             .arg(out_file, ofile.errorString()));
    return;
  }

  ofile.write("\"Block name\",\"Block id\",\"Count\"\n");
  const auto current_version =
      dynamic_cast<SCWind*>(this->parent())->selected_version();
  for (const auto& mat : this->mat_list) {
    QString line =
        QStringLiteral("\"%1\",\"%2\",%3\n")
            .arg(mat.blk->getNameEN(), mat.blk->idForVersion(current_version))
            .arg(mat.count);
    ofile.write(line.toLocal8Bit());
  }
  ofile.close();
}

MaterialModel::MaterialModel(PreviewWind* parent)
    : QAbstractTableModel(parent), pwind(parent) {}

MaterialModel::~MaterialModel() {}

void MaterialModel::refresh() noexcept {
  emit this->dataChanged(
      this->index(0, 0),
      this->index(this->rowCount() - 1, this->columnCount() - 1));
}

QString format_num(int num, int stack_size) noexcept {
  assert(stack_size > 0);
  const int sb_size = stack_size * 27;

  const int sb_num = num / sb_size;
  num -= sb_num * sb_size;

  const int stack_num = num / stack_size;
  num -= stack_num * stack_size;

  const int left_num = num;

  if (sb_num > 0) {
    return MaterialModel::tr("%1 盒 + %2 组 + %3 个")
        .arg(sb_num)
        .arg(stack_num)
        .arg(left_num);
  }

  if (stack_num > 0) {
    return MaterialModel::tr("%1 组 + %2 个").arg(stack_num).arg(left_num);
  }

  return MaterialModel::tr("%1 个").arg(left_num);
}

QVariant MaterialModel::data(const QModelIndex& qmi, int role) const noexcept {
  if (!qmi.isValid()) {
    return {};
  }

  const int r = qmi.row();
  const int c = qmi.column();

  if (r >= (int)this->mat_list->size()) {
    return {};
  }
  if (c >= 2) {
    return {};
  }

  using it_t = std::vector<material_item>::const_iterator;
  std::vector<it_t> its;
  {
    its.reserve(this->mat_list->size());

    for (auto it = this->mat_list->begin(); it != this->mat_list->end(); ++it) {
      its.emplace_back(it);
    }

    const auto sort_opt = this->pwind->current_sort_option();

    if (sort_opt != PreviewWind::sort_option::no_sort) {
      auto sort_fun = [sort_opt](it_t a, it_t b) -> bool {
        if (a->count == b->count) {
          return a->blk < b->blk;
        }

        if (sort_opt == PreviewWind::sort_option::ascending) {
          return a->count < b->count;
        }

        return a->count > b->count;
      };

      std::sort(its.begin(), its.end(), sort_fun);
    }
  }

  const auto& mat = *its[r];

  if (role == Qt::ItemDataRole::DisplayRole) {
    if (c == 0) {
      const auto lang =
          dynamic_cast<const SCWind*>(this->pwind->parent())->lang();
      if (lang == ::SCL_language::Chinese) {
        return QString::fromUtf8(mat.blk->getNameZH());
      } else {
        return QString::fromUtf8(mat.blk->getNameEN());
      }
    }

    if (c == 1) {
      if (!this->pwind->is_unit_stack()) {
        return QString::number(mat.count);
      }
      const int stack_size = std::max<uint8_t>(1, mat.blk->getStackSize());
      return format_num(mat.count, stack_size);
    }
  }

  if (role == Qt::ItemDataRole::DecorationRole) {
    if (c == 0) {
      QImage img{16, 16, QImage::Format_ARGB32};
      mat.blk->getImage((uint32_t*)img.scanLine(0));
      return QIcon{QPixmap::fromImage(img)};
    }
  }

  return {};
}

int MaterialModel::columnCount(const QModelIndex&) const noexcept { return 2; }

int MaterialModel::rowCount(const QModelIndex&) const noexcept {
  // const int rows = this->mat_list->size();
  return this->mat_list->size();
}