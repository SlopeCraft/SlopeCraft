#include "processMapFiles.h"

#include <zlib.h>
#include <vector>
#include <filesystem>
#include <fstream>

#include <iostream>

bool uncompress_map_file(const char * filename,std::vector<uint8_t>*const dest) {
    if(filename==nullptr||strlen(filename)<=0) {
        return false;
    }
    if((!std::filesystem::exists(filename))||(!std::filesystem::is_regular_file(filename))) {
        return false;
    }

    if(dest==nullptr) {
        return false;
    }

    dest->clear();

    const int source_size=std::filesystem::file_size(filename);
    dest->reserve(4*source_size);

    gzFile gz_file=::gzopen(filename,"rb");
    while(true) {
        dest->emplace_back();
        if(::gzfread(&dest->back(),1,1,gz_file)<=0) {
            dest->pop_back();
            break;
        }
    }

    ::gzclose(gz_file);
    dest->shrink_to_fit();

/*
    std::vector<uint8_t> source;
    source.resize(sourceSize);

    {
        std::fstream file;
        file.open(filename,std::ios::in|std::ios::binary);
        file.read((char*)source.data(),sourceSize);
        file.close();
    }

    dest->reserve(2.5*sourceSize);
    uLongf destLength=dest->capacity();
    while(true) {
        int error=::uncompress(dest->data(),&destLength,source.data(),source.size());
        switch (error) {
        case Z_OK:
        break;
        case Z_BUF_ERROR:
            dest->reserve(dest->capacity()*2);
            destLength=dest->capacity();
            continue;
        default:
            std::cout<<"Failed to inflate file : "<<filename<<" , error code is "<<error<<std::endl;
            exit(1);
            break;
        }
        break;
    }

    if(destLength<=dest->capacity()) {
        dest->resize(destLength);
    }*/

    return true;
}
