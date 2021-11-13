#include "TokiTask.h"

bool TokiTask::canExportLite=true;
const bool TokiTask::canExportData=true;

TokiTask::TokiTask() {

}

TokiTask::~TokiTask() {

}

uint TokiTask::mapCount() const {
    return  std::ceil(rows()/128.0)*std::ceil(cols()/128.0);
}

ushort TokiTask::rows() const {
    return TokiRow(src_imageSize);
}

ushort TokiTask::cols() const {
    return TokiCol(src_imageSize);
}

void TokiTask::clear() {
    src_imageName="";
    src_imageSize=TokiRC(0,0);
    dst_DataFileName="";
    dst_liteFileName="";
    dst_beginSeqNum=0;
}
