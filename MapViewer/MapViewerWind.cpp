#include "MapViewerWind.h"
#include "ui_MapViewerWind.h"

#include "processMapFiles.h"

#include <QFileDialog>
#include <QMessageBox>

#include <list>
#include <mutex>

static const int current_max_base_color=61;

std::array<ARGB,256> make_map_LUT();
std::array<ARGB,256> make_inverse_map_LUT(const std::array<ARGB,256> &);

const std::array<ARGB,256> map_color_to_ARGB=make_map_LUT();
const std::array<ARGB,256> inverse_map_color_to_ARGB
    =make_inverse_map_LUT(map_color_to_ARGB);

std::array<ARGB,256> make_map_LUT() {
    std::array<ARGB,256> result;

    result.fill(0x7FFF0000);

    const Eigen::Map<const Eigen::Array<float,256,3>> src(SlopeCraft::RGBBasicSource);

    for(int row_idx=0;row_idx<256;row_idx++) {
        ARGB a,r,g,b;
        const int base_color=row_idx%64;
        const int shade=row_idx/64;
        const int map_color=base_color*4+shade;

        if(base_color>current_max_base_color) {
            continue;
        }
        r=std::min(255U,uint32_t(255*src(row_idx,0)));
        g=std::min(255U,uint32_t(255*src(row_idx,1)));
        b=std::min(255U,uint32_t(255*src(row_idx,2)));

        a=(base_color!=0)?(255):(0);

        result[map_color]=(a<<24)|(r<<16)|(g<<8)|(b);
    }

    return result;
}

std::array<ARGB,256> make_inverse_map_LUT(const std::array<ARGB,256> & src) {
    std::array<ARGB,256> result;
    for(size_t idx=0;idx<src.size();idx++) {
        const ARGB argb=src[idx];
        ARGB r=(argb>>16)&0xFF;
        ARGB g=(argb>>8)&0xFF;
        ARGB b=(argb)&0xFF;

        r=255-r;
        g=255-g;
        b=255-b;
        result[idx]=(0xFF<<24)|(r<<16)|(g<<8)|(b);
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

    connect(ui->combobox_select_map,&QComboBox::currentIndexChanged,
            this,&MapViewerWind::render_single_image);
    connect(ui->spinbox_pixel_size,&QSpinBox::valueChanged,
            this,&MapViewerWind::render_single_image);
    connect(ui->radio_show_base_color,&QRadioButton::clicked,
            this,&MapViewerWind::render_single_image);
    connect(ui->radio_show_color_only,&QRadioButton::clicked,
            this,&MapViewerWind::render_single_image);
    connect(ui->radio_show_map_color,&QRadioButton::clicked,
            this,&MapViewerWind::render_single_image);
    connect(ui->radio_show_shade,&QRadioButton::clicked,
            this,&MapViewerWind::render_single_image);
    connect(ui->checkbox_single_map_show_grid,&QCheckBox::clicked,
            this,&MapViewerWind::render_single_image);

}

MapViewerWind::~MapViewerWind()
{
    delete ui;
}

void MapViewerWind::update_contents() {
    reshape_tables();
    ui->label_show_map_count->setText(tr("地图数：")+QString::number(this->maps.size()));

    //update combo box
    ui->combobox_select_map->clear();
    for(auto & map : this->maps) {
        ui->combobox_select_map->addItem(map.filename);
    }
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
    ui->combobox_select_map->clear();
    ui->label_show_single_map->setPixmap(QPixmap());
    ui->label_show_map_count->setText(tr("请选择地图文件"));
}


void MapViewerWind::render_single_image() {
    //ui->label_show_single_map->setPixmap(QPixmap());

    static int prev_pixel_size=-1;
    static int prev_idx=-1;
    static uint8_t prev_show_grid=2;

    const int current_idx=ui->combobox_select_map->currentIndex();
    if(current_idx<0 || current_idx>=int(this->maps.size())) {
        ui->label_show_single_map->setPixmap(QPixmap());
        prev_idx=-1;
        return;
    }

    const int pixel_size=ui->spinbox_pixel_size->value();
    const int rows=pixel_size*128,cols=pixel_size*128;
    const uint8_t show_grid=ui->checkbox_single_map_show_grid->isChecked();

    const bool is_color_only_image_changed=
            (pixel_size!=prev_pixel_size)||(prev_idx!=current_idx)||(show_grid!=prev_show_grid);
    // update previous
    prev_pixel_size=pixel_size;
    prev_idx=current_idx;
    prev_show_grid=show_grid;

    static QImage new_image;

    if(is_color_only_image_changed) {
        //cout<<"repaint"<<endl;
    }
    else {
        //cout<<"Don't repaint"<<endl;
    }

    if(is_color_only_image_changed) { // if color only image is changed, repaint it
        new_image=QImage(cols,rows,QImage::Format_ARGB32);
        new_image.fill(QColor(255,255,255,255));

        Eigen::Map<Eigen::Array<ARGB,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor>>
                map_new_image(reinterpret_cast<ARGB*>(new_image.scanLine(0)),rows,cols);

        const Eigen::Map<const Eigen::Array<ARGB,128,128,Eigen::RowMajor>>
                image_map(reinterpret_cast<ARGB*>(this->maps[current_idx].image.scanLine(0)));

        for(int c=0;c<image_map.cols();c++) {
            for(int r=0;r<image_map.rows();r++) {
                map_new_image.block(pixel_size*r,pixel_size*c,pixel_size,pixel_size)
                        .fill(image_map(r,c));
            }
        }

        if(pixel_size>=4&&show_grid) {
            for(int map_r_idx=0;map_r_idx<128;map_r_idx++) {
                for(int img_c_idx=0;img_c_idx<cols;img_c_idx++) {
                    const uint8_t map_color=
                            this->maps[current_idx].content()(map_r_idx,img_c_idx/pixel_size);
                    map_new_image(map_r_idx*pixel_size,img_c_idx)=
                            ::inverse_map_color_to_ARGB[map_color];
                }
            }


            for(int map_c_idx=0;map_c_idx<128;map_c_idx++) {
                for(int img_r_idx=0;img_r_idx<rows;img_r_idx++) {
                    const uint8_t map_color=
                            this->maps[current_idx].content()(img_r_idx/pixel_size,map_c_idx);
                    map_new_image(img_r_idx,map_c_idx*pixel_size)=
                            ::inverse_map_color_to_ARGB[map_color];
                }
            }
        }
    }

    ui->label_show_single_map->setPixmap(QPixmap::fromImage(new_image));

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

#pragma omp parallel for
    for(map & map : this->maps) {
        map.image=QImage(128,128,QImage::Format::Format_ARGB32);
        Eigen::Map<Eigen::Array<ARGB,128,128,Eigen::RowMajor>>
                image_map(reinterpret_cast<ARGB*>(map.image.scanLine(0)));
        for(int64_t idx=0;idx<map.map_content->size();idx++) {
            image_map(idx)=map_color_to_ARGB[map.map_content->operator()(idx)];
        }
    }


    update_contents();

}

