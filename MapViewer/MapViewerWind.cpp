#include "MapViewerWind.h"
#include "ui_MapViewerWind.h"
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
            this,&MapViewerWind::reshape_tables);
    connect(ui->spinbox_cols,&QSpinBox::valueChanged,
            this,&MapViewerWind::reshape_tables);
    connect(ui->radio_is_row_major,&QRadioButton::clicked,
            this,&MapViewerWind::reshape_tables);
    connect(ui->radio_is_col_major,&QRadioButton::clicked,
            this,&MapViewerWind::reshape_tables);

    connect(ui->botton_clear,&QPushButton::clicked,
            this,&MapViewerWind::clear_all);
}

MapViewerWind::~MapViewerWind()
{
    delete ui;
}


void MapViewerWind::reshape_tables() {
    const int rows=ui->spinbox_rows->value();
    const int cols=ui->spinbox_cols->value();
    const bool is_col_major=ui->radio_is_col_major->isChecked();

    this->clear_all();

    ui->table_display_filename->setRowCount(rows);
    ui->table_display_filename->setColumnCount(cols);

    for(int r=0;r<rows;r++) {
        for(int c=0;c<cols;c++) {
            //QString str="("+QString::number(r)+" , "+QString::number(c)+")";

            QString str;
            if(is_col_major) {
                str=QString::number(r+c*rows);
            }
            else {
                str=QString::number(c+r*cols);
            }

            QTableWidgetItem * item=new QTableWidgetItem(str);
            ui->table_display_filename->setItem(r,c,item);

        }
    }
}

void MapViewerWind::clear_all() {
    ui->table_display_filename->clearContents();
}
