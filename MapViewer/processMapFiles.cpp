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

    const size_t source_size=std::filesystem::file_size(filename);
    dest->reserve(4*source_size);

    gzFile gz_file=::gzopen(filename,"rb");

    if(gz_file==nullptr) {
        return false;
    }

    while(true) {
        dest->emplace_back();
        if(::gzfread(&dest->back(),1,1,gz_file)<=0) {
            dest->pop_back();
            break;
        }
    }

    ::gzclose(gz_file);
    dest->shrink_to_fit();

    return true;
}

const uint8_t * find_color_begin(const std::vector<uint8_t>&inflated) {
    if(inflated.size()<=128*128*sizeof(char)) {
        return nullptr;
    }

    //std::vector<const uint8_t *> possiblePtrs;

    constexpr uint8_t feature[]={0x63,0x6F,0x6C,0x6F,0x73,0x00,0x00,0x40,0x00};
    constexpr size_t feature_length=sizeof(feature)/sizeof(uint8_t);
    for(const uint8_t * ptr=inflated.data();ptr!=inflated.data()+inflated.size();ptr++) {
        if(*ptr!=*feature) {
            continue;
        }
        bool same=true;
        for(size_t idx=1;idx<feature_length;idx++) {
            same=same&&(ptr[idx]==feature[idx]);
        }

        if(same) {
            const size_t left_bytes=inflated.data()+inflated.size()-ptr-feature_length;
            if(left_bytes>=128*128*sizeof(uint8_t)) {
                return ptr+feature_length;
            }
        }
        else {
            continue;
        }
    }

    return nullptr;
}
