#include "MapViewerWind.h"
#include "ui_MapViewerWind.h"

#include "processMapFiles.h"

#include <QFileDialog>
#include <QMessageBox>

#include <list>
#include <mutex>


std::array<ARGB,256> make_map_LUT();

const std::array<ARGB,256> map_color_to_ARGB=make_map_LUT();

std::array<ARGB,256> make_map_LUT() {
    std::array<ARGB,256> result;


    const Eigen::Map<const Eigen::Array<float,256,3>> src(SlopeCraft::RGBBasicSource);

    for(int row_idx=0;row_idx<256;row_idx++) {
        ARGB a,r,g,b;
        const int map_color=(row_idx<<2)|(row_idx>>6);

        r=std::min(255U,uint32_t(255*src(row_idx,0)));
        g=std::min(255U,uint32_t(255*src(row_idx,1)));
        b=std::min(255U,uint32_t(255*src(row_idx,2)));

        a=(map_color&0b11111100)?(255):(0);

        result[map_color]=(a<<24)|(r<<16)|(g<<8)|(b);
    }

    return result;
}

MapViewerWind::MapViewerWind(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MapViewerWind)
{
    ui->setupUi(this);

    connect(ui->spinbox_rows,&QSpinBox::valueChanged,
            this,&MapViewerWind::update_contents);
    connect(ui->spinbox_cols,&QSpinBox::valueChanged,
            this,&MapViewerWind::update_contents);
    connect(ui->radio_is_row_major,&QRadioButton::clicked,
            this,&MapViewerWind::update_contents);
    connect(ui->radio_is_col_major,&QRadioButton::clicked,
            this,&MapViewerWind::update_contents);

    connect(ui->botton_clear,&QPushButton::clicked,
            this,&MapViewerWind::clear_all);
}

MapViewerWind::~MapViewerWind()
{
    delete ui;
}

void MapViewerWind::update_contents() {
    reshape_tables();
    ui->label_show_map_count->setText(tr("地图数：")+QString::number(this->maps.size()));
}

void MapViewerWind::reshape_tables() {
    const int rows=ui->spinbox_rows->value();
    const int cols=ui->spinbox_cols->value();
    const bool is_col_major=ui->radio_is_col_major->isChecked();

    this->clear_all();

    ui->table_display_filename->setRowCount(rows);
    ui->table_display_filename->setColumnCount(cols);

    for(int idx=0;idx<int(this->maps.size());idx++) {
        const int r=(is_col_major)?(idx%rows):(idx/cols);
        const int c=(is_col_major)?(idx/rows):(idx%cols);

        QTableWidgetItem * item=new QTableWidgetItem(this->maps[idx].filename);

        ui->table_display_filename->setItem(r,c,item);
    }

}

void MapViewerWind::clear_all() {
    ui->table_display_filename->clearContents();
}

void MapViewerWind::on_button_load_maps_clicked() {

    QStringList filenames=
            QFileDialog::getOpenFileNames(this,tr("选择地图数据文件"),"","map_*.dat");
    if(filenames.size()<=0) {
        return;
    }

    this->clear_all();
    this->maps.clear();
    this->maps.resize(filenames.size());

    std::list<std::pair<QString,std::string>> error_list;
    std::mutex lock;
#pragma omp parallel for
    for(int idx=0;idx<filenames.size();idx++) {

        std::string error_info;
        if(!process_map_file(filenames[idx].toLocal8Bit().data(),
                             (this->maps[idx].map_content).get(),
                             &error_info)) {
            lock.lock();

            error_list.emplace_back(filenames[idx],error_info);

            this->maps[idx].filename="";

            lock.unlock();
        }
        else {
            const int last_idx_of_reverse_slash=filenames[idx].lastIndexOf('\\');
            const int last_idx_of_slash=filenames[idx].lastIndexOf('/');
            const int last_idx_of_seperator=std::max(last_idx_of_reverse_slash,last_idx_of_slash);

            const int basename_length=filenames[idx].length()-last_idx_of_seperator-1;

            this->maps[idx].filename=filenames[idx].last(basename_length);
        }
    }

    for(const auto & error : error_list) {
        QMessageBox::StandardButton user_clicked=
        QMessageBox::warning(this,tr("加载地图文件失败"),
                                 tr("出错的文件：")+error.first+"\n错误信息：\n"
                                 +QString::fromLocal8Bit(error.second.data()),
                             {QMessageBox::StandardButton::Ignore,QMessageBox::StandardButton::NoToAll},
                             QMessageBox::StandardButton::Ignore
                             );
        if(user_clicked==QMessageBox::StandardButton::Ignore) {
            continue;
        }
        else {
            break;
        }
    }

    for(auto it=this->maps.begin();it!=this->maps.end();) {
        if(it->filename.isEmpty()) {
            it=this->maps.erase(it);
        }
        else {
            ++it;
        }
    }

    update_contents();

}

