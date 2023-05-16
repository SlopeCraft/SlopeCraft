#include "PoolModel.h"
#include <QMimeData>
#include <list>
#include <ranges>
#include <QPainter>
#include <QMessageBox>

PoolModel::PoolModel(QObject* parent, task_pool_t* poolptr)
    : QAbstractListModel(parent), pool(poolptr) {}

PoolModel::~PoolModel() {}

const QPixmap& PoolModel::icon_empty() noexcept {
  static QPixmap img{":/images/empty.png"};
  return img;
}

const QPixmap& PoolModel::icon_converted() noexcept {
  static QPixmap img{":/images/converted.png"};
  return img;
}

QVariant PoolModel::data(const QModelIndex& idx, int role) const {
  const auto& task = this->pool->at(idx.row());
  if (role == Qt::ItemDataRole::DisplayRole) {
    return task.filename;
  }

  if (role == Qt::ItemDataRole::DecorationRole) {
    assert(this->_listview != nullptr);
    if (this->_listview->viewMode() == QListView::ViewMode::ListMode) {
      return QVariant{};
    }
    auto raw_image = QPixmap::fromImage(task.original_image);
    auto img = raw_image.scaledToWidth(this->_listview->size().width());

    if (!task.is_converted) {
      this->draw_icon(img, icon_empty(), 0);
    } else {
      this->draw_icon(img, icon_converted(), 0);
    }

    return QIcon{img};
  }

  return QVariant{};
}

QPixmap scale_up_to_3232(const QPixmap& original_pixmap,
                         QSize min_size) noexcept {
  const QSize old_size = original_pixmap.size();
  const QSize new_size{std::min(old_size.width(), min_size.width()),
                       std::min(old_size.height(), min_size.height())};
  if (old_size == new_size) {
    return original_pixmap;
  }
  QImage new_img{new_size, QImage::Format_ARGB32};
  memset(new_img.scanLine(0), 0, new_img.sizeInBytes());
  {
    QPainter p{&new_img};
    p.drawPixmap(0, 0, original_pixmap);
    p.end();
  }

  return QPixmap::fromImage(new_img);
}

void PoolModel::draw_icon(QPixmap& image, const QPixmap& icon, int index,
                          QWidget* ptr_to_report_error) noexcept {
  assert(index >= 0);
  if (icon.size() != QSize{32, 32}) [[unlikely]] {
    QMessageBox::critical(
        ptr_to_report_error, QObject::tr("绘制图标时发现错误"),
        tr("被绘制的图标尺寸应当是32*32，但实际上是%1*%"
           "2。这属于SlopeCraft内部错误，请向开发者反馈。SlopeCraft必须崩溃。")
            .arg(icon.size().height())
            .arg(icon.size().width()));
    abort();
    return;
  }
  {
    const QSize expected_min_size{(index + 1) * 32, 32};

    if (image.height() < expected_min_size.height() ||
        image.width() < expected_min_size.width()) [[unlikely]] {
      image = scale_up_to_3232(image, expected_min_size);
    }
  }
  QPainter painter{&image};

  const QSize img_size = image.size();

  const int x = img_size.width() - (index + 1) * 32;
  const int y = img_size.height() - 32;
  painter.drawPixmap(x, y, icon);
  painter.end();
}

CvtPoolModel::CvtPoolModel(QObject* parent, task_pool_t* poolptr)
    : PoolModel(parent, poolptr) {}

CvtPoolModel::~CvtPoolModel() {}

Qt::DropActions CvtPoolModel::supportedDropActions() const {
  return Qt::DropActions{Qt::DropAction::MoveAction,
                         Qt::DropAction::CopyAction};
}

Qt::ItemFlags CvtPoolModel::flags(const QModelIndex& index) const {
  Qt::ItemFlags defaultFlags = QAbstractListModel::flags(index);

  if (index.isValid())
    return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
  else
    return Qt::ItemIsDropEnabled | defaultFlags;
}

QStringList CvtPoolModel::mimeTypes() const {
  return QStringList{"text/plain", "image/png"};
}

const char mime_data_type[] = "application/slopecraft_pool_index";

QByteArray encode_indices(const std::vector<int>& temp) noexcept {
  QByteArray qba{reinterpret_cast<const char*>(temp.data()),
                 qsizetype(temp.size() * sizeof(int))};
  return qba;
}

std::vector<int> decode_indices(const QByteArray& qbav) noexcept {
  if (qbav.size() % sizeof(int) != 0) {
    return {};
  }

  const int size = qbav.size() / sizeof(int);

  const int* const data = reinterpret_cast<const int*>(qbav.data());

  std::vector<int> ret{data, data + size};
  return ret;
}

QMimeData* CvtPoolModel::mimeData(const QModelIndexList& indexes) const {
  std::vector<int> temp;
  temp.reserve(indexes.size());

  for (const auto& midx : indexes) {
    temp.emplace_back(midx.row());
  }

  QMimeData* ret = new QMimeData;

  ret->setData(mime_data_type, encode_indices(temp));

  return ret;
}

bool CvtPoolModel::canDropMimeData(const QMimeData* data, Qt::DropAction, int,
                                   int col, const QModelIndex& parent) const {
  if (parent.isValid()) {
    return false;
  }

  if (col > 0) {
    return false;
  }

  if (!data->hasFormat(mime_data_type)) {
    return true;
  }

  const int bytes = data->data(mime_data_type).size();

  if (bytes % sizeof(int) != 0) {
    return false;
  }

  // disable moving multiple items, because the behavior is incorrect
  if (bytes / sizeof(int) == 1) {
    return true;
  }

  return false;
}

template <typename it_t>
void iterator_add(it_t& it, int n) noexcept {
  assert(n >= 0);
  for (int i = 0; i < n; i++) {
    ++it;
  }
}

template <typename T>
void map_indices(std::vector<T>& pool, std::vector<int> moved_indices,
                 int begin_idx) noexcept {
  std::list<T> temp_pool;
  for (T& t : pool) {
    temp_pool.emplace_back(std::move(t));
  }

  std::sort(moved_indices.begin(), moved_indices.end());
  std::vector<typename std::list<T>::iterator> src_it_vec;
  src_it_vec.reserve(moved_indices.size());
  {
    int idx = 0;
    auto it = temp_pool.begin();
    for (int sidx : moved_indices) {
      const int offset = sidx - idx;
      assert(offset >= 0);
      iterator_add(it, offset);

      src_it_vec.emplace_back(it);
    }
  }

  auto begin_it = temp_pool.begin();

  iterator_add(begin_it, begin_idx);

  for (auto srcit : src_it_vec) {
    temp_pool.emplace(begin_it, *srcit);
  }

  for (auto srcit : src_it_vec) {
    temp_pool.erase(srcit);
  }

  pool.clear();
  for (auto& t : temp_pool) {
    pool.emplace_back(std::move(t));
  }
}

bool CvtPoolModel::dropMimeData(const QMimeData* data, Qt::DropAction action,
                                int row, int column,
                                const QModelIndex& parent) {
  if (!this->canDropMimeData(data, action, row, column, parent)) {
    return false;
  }

  if (action == Qt::IgnoreAction) {
    return true;
  }

  int begin_row;

  if (row != -1)
    begin_row = row;
  else if (parent.isValid())
    begin_row = parent.row();
  else
    begin_row = this->rowCount(QModelIndex{});

  {
    auto src_indices = decode_indices(data->data(mime_data_type));

    if (src_indices.size() <= 0) {
      return true;
    }
    map_indices(*this->pool, src_indices, begin_row);
  }
  this->refresh();

  return true;
}

ExportPoolModel::ExportPoolModel(QObject* parent, task_pool_t* poolptr)
    : PoolModel(parent, poolptr) {}

ExportPoolModel::~ExportPoolModel() {}

QVariant ExportPoolModel::data(const QModelIndex& midx, int role) const {
  const int fidx = this->export_idx_to_full_idx(midx.row());
  assert(fidx >= 0);
  assert(fidx < (int)this->pool->size());

  const auto& task = this->pool->at(fidx);

  if (role == Qt::ItemDataRole::DisplayRole) {
    return task.filename;
  }

  if (role == Qt::ItemDataRole::DecorationRole) {
    assert(this->_listview != nullptr);
    if (this->_listview->viewMode() == QListView::ViewMode::ListMode) {
      return QVariant{};
    }
    auto raw_image = QPixmap::fromImage(task.original_image);
    auto img = raw_image.scaledToWidth(this->_listview->size().width());
    if (!task.is_built) {
      this->draw_icon(img, icon_empty(), 0);
    } else {
      this->draw_icon(img, icon_converted(), 0);
    }
    return QIcon{img};
  }

  return QVariant{};
}