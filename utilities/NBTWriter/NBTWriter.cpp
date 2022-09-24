#include "NBTWriter.h"
#include <zlib.h>

using namespace NBT::internal;

bool NBTWriterBase_nocompress::open(const char *newFileName) noexcept {

  if (file != nullptr) {
    return false;
  }

  // FILE *newfile = ::fopen(newFileName, "wb");

  FILE *newfile = NULL;
  ::fopen_s(&newfile, newFileName, "wb");

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

  if (file != nullptr) {
    return false;
  }

  gzFile newfile = ::gzopen(newFileName, "wb");

  if (newfile == NULL) {
    return false;
  }

  file = newfile;

  bytesWritten = 0;

  constexpr char head[3] = {10, 0, 0};

  write_data(head, sizeof(head));

  return true;
}

int NBTWriterBase_nocompress::write_data(const void *data,
                                         const size_t bytes) noexcept {
  ::fwrite(data, sizeof(char), bytes, file);

  bytesWritten += bytes;

  return bytes;
}

int NBTWriterBase_gzip::write_data(const void *data,
                                   const size_t bytes) noexcept {
  ::gzfwrite(data, sizeof(char), bytes, file);

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