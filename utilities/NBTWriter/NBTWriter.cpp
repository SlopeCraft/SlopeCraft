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

#include "NBTWriter.h"
#include <stdio.h>
#include <zlib.h>


using namespace NBT::internal;

bool NBTWriterBase_nocompress::open(const char *newFileName) noexcept {

  if (file != nullptr) {
    return false;
  }

  // FILE *newfile = ::fopen(newFileName, "wb");

  FILE *newfile = NULL;
  newfile = fopen(newFileName, "wb");

  if (newfile == NULL) {
    return false;
  }

  file = newfile;

  bytesWritten = 0;

  constexpr char head[3] = {10, 0, 0};

  write_data(head, sizeof(head));

  return true;
}

bool NBTWriterBase_gzip::open(const char *newFileName) noexcept {

  if (this->file != nullptr) {
    return false;
  }

  gzFile newfile = gzopen(newFileName, "wb");

  if (newfile == NULL) {
    return false;
  }

  this->file = newfile;

  bytesWritten = 0;

  constexpr char head[3] = {10, 0, 0};

  write_data(head, sizeof(head));

  return true;
}

int NBTWriterBase_nocompress::write_data(const void *data,
                                         const size_t bytes) noexcept {
  fwrite(data, sizeof(char), bytes, file);

  bytesWritten += bytes;

  return bytes;
}

int NBTWriterBase_gzip::write_data(const void *data,
                                   const size_t bytes) noexcept {
  gzfwrite(data, sizeof(char), bytes, file);

  bytesWritten += bytes;

  return bytes;
}

void NBTWriterBase_nocompress::close_file() noexcept {
  fclose(file);
  file = NULL;
}

void NBTWriterBase_gzip::close_file() noexcept {
  gzclose(file);
  file = NULL;
}