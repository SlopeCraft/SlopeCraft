/*
 Copyright © 2021-2026  TokiNoBug
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

#ifndef SCL_NBTWRITER_H
#define SCL_NBTWRITER_H

// #include <bits/endian>
#include <assert.h>
#include <stack>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <type_traits>
#include <vector>
#include <string_view>
#include <stdlib.h>

// #include <iostream>

// using std::cout,std::endl;

struct gzFile_s;

namespace NBT {

namespace internal {
class NBTWriterBase_nocompress {
 protected:
  uint64_t bytesWritten{0};

  int write_data(const void *data, const size_t bytes) noexcept;

 private:
  FILE *file{NULL};

 public:
  /**
   * \brief open a file
   * \param newFileName the file to be opened
   * \return If openning succeeds
   */
  bool open(const char *newFileName) noexcept;
  void close_file() noexcept;

  /**
   * \brief file pointer
   * \return file pointer
   */
  inline FILE *file_ptr() noexcept { return file; }

  /**
   * \brief file pointer
   * \return constant file pointer
   */
  inline const FILE *file_ptr() const noexcept { return file; }

  /**
   * \brief If is file opened
   * \return If is file opened
   */
  inline bool is_open() const noexcept { return file != NULL; }
};

class NBTWriterBase_gzip {
 protected:
  uint64_t bytesWritten{0};

  int write_data(const void *data, const size_t bytes) noexcept;

 private:
  gzFile_s *file{NULL};

 public:
  /**
   * \brief open a file
   * \param newFileName the file to be opened
   * \return If openning succeeds
   */
  bool open(const char *newFileName) noexcept;
  /**
   * \brief If is file opened
   * \return If is file opened
   */
  inline bool is_open() const noexcept { return file != NULL; }

  void close_file() noexcept;

  /**
   * \brief file pointer
   * \return file pointer
   */
  inline gzFile_s *file_ptr() noexcept { return file; }

  /**
   * \brief file pointer
   * \return constant file pointer
   */
  inline const gzFile_s *file_ptr() const noexcept { return file; }
};

}  // namespace internal

constexpr char idEnd = 0;
constexpr char idByte = 1;
constexpr char idShort = 2;
constexpr char idInt = 3;
constexpr char idLong = 4;
constexpr char idFloat = 5;
constexpr char idDouble = 6;
constexpr char idByteArray = 7;
constexpr char idString = 8;
constexpr char idList = 9;
constexpr char idCompound = 10;
constexpr char idIntArray = 11;
constexpr char idLongArray = 12;

enum tagType : char {
  End = idEnd,
  Byte = idByte,
  Short = idShort,
  Int = idInt,
  Long = idLong,
  Float = idFloat,
  Double = idDouble,
  ByteArray = idByteArray,
  String = idString,
  List = idList,
  Compound = idCompound,
  IntArray = idIntArray,
  LongArray = idLongArray
};

/**
 * \brief Convert variables via little endian and big endian.
 * \param t To be converted
 * \return Converted
 */
template <typename T>
inline T convertLEBE(T t) {
  uint8_t *ptr = reinterpret_cast<uint8_t *>(&t);

  for (int idx = 0; idx * 2 + 1 <= int(sizeof(T)); idx++) {
    std::swap(ptr[idx], ptr[sizeof(T) - idx - 1]);
  }

  return t;
}

/**
 * \brief The NBTWriter class
 */
template <bool is_nbt_compressed>
class NBTWriter
    : public std::conditional_t<is_nbt_compressed,
                                typename internal::NBTWriterBase_gzip,
                                internal::NBTWriterBase_nocompress> {
 public:
  struct task_t {
    task_t() : currentTagType(tagType::End), taskSize(0) {}
    task_t(const tagType type, int size)
        : currentTagType(type), taskSize(size) {}

    tagType currentTagType;
    int taskSize;
  };

 private:
  std::stack<task_t> tasks;

 public:
  /**
   * \brief Default constructor
   */
  NBTWriter() : tasks() {}

  /**
   * \brief Construct and open a file
   * \param fileName The file to be opened.
   */
  NBTWriter(const char *fileName) : tasks() { this->open(fileName); }

  /**
   * \brief Close the file and destruct
   */
  ~NBTWriter() {
    if (this->is_open()) {
      close();
    }
  }

  /**
   * \brief Close the file and automatically fill nbts.
   * \return If closing succeeds
   */
  bool close() {
    if (!this->is_open()) {
      return false;
    }

    if (!tasks.empty()) emergencyFill();

    constexpr char fileTail[1] = {idEnd};

    this->write_data(fileTail, sizeof(fileTail));

    this->close_file();

    return true;
  }

  /**
   * \brief emergencyFill fils unfinished nbt tags automatically so that the NBT
   * file can be loaded at least. \return Bytes written
   */
  int emergencyFill() {
    if (tasks.empty()) {
      return 0;
    }

    int bytes = 0;

    while (!tasks.empty()) {
      if (isInCompound()) {
        bytes += endCompound();
        continue;
      }

      // cout<<"tasks.size() = "<<tasks.size()<<endl;

      switch (currentType()) {
        case End:

          exit(114514);

          continue;
        case Byte:

          bytes += writeByte("autoByte", 114);

          continue;
        case Short:

          bytes += writeShort("autoShort", 514);

          continue;
        case Int:

          bytes += writeInt("autoInt", 114514);

          continue;
        case Long:

          bytes += writeLong("autoLong", 1919810);

          continue;
        case Float:

          bytes += writeFloat("autoFloat", 114.514);

          continue;
        case Double:

          bytes += writeDouble("autoDouble", 1919810.114514);

          continue;
        case String:

          bytes += writeString("autoString", "FuckYou!");

          continue;
        case List:

          bytes += writeListHead("autoList", Int, 1);

          continue;
        case Compound:

          bytes += writeCompound("autoCompound");

          continue;
        case ByteArray:
          bytes += writeByteArrayHead("autoByteArray", 1);

          continue;
        case IntArray:

          bytes += writeIntArrayHead("autoIntArray", 1);

          continue;
        case LongArray:

          bytes += writeLongArrayHead("autoLongArray", 1);

          continue;
      }
    }

    bytes += writeString("TokiNoBug'sWarning",
                         "There's sth wrong with ur NBTWriter, the file format "
                         "is completed automatically instead of manually.");

    return bytes;
  }

 private:
 public:
  /**
   * \brief Whether the NBTWriter is finishing a compound tag
   * \return Whether the NBTWriter is finishing a compound tag
   */
  inline bool isInCompound() const {
    return tasks.empty() || (tasks.top().currentTagType == tagType::End);
  }

  /**
   * \brief Whether the NBTWriter is finishing a list tag or array tag
   * \return Whether the NBTWriter is finishing a list tag or array tag
   */
  inline bool isInListOrArray() const { return !isInCompound(); }

  /**
   * \brief Tell if you can write this tag into current list/ array.
   * \param type The tag type you want to write in
   * \return Whether you can write
   */
  inline bool typeMatch(const tagType type) const {
    return type == tasks.top().currentTagType;
  }

  inline bool isListOrArrayFinished() const {
    return tasks.top().taskSize <= 0;
  }

 private:
  inline void onElementWritten() {
    if (!isInListOrArray()) {
      return;
    }

    if (!isListOrArrayFinished()) {
      tasks.top().taskSize--;
    }

    tryEndList();
  }

  inline void tryEndList() {
    if (!isInListOrArray()) {
      return;
    }

    while (!tasks.empty()) {
      // cout<<"stack size = "<<tasks.size()<<endl;
      if (!isInListOrArray()) {
        return;
      }

      if (isListOrArrayFinished()) {
        tasks.pop();
        continue;
      } else {
        return;
      }
    }
  }

  inline int writeEnd() {
    this->write_data(&idEnd, 1);
    return 1;
  }

 public:
  /**
   * \brief writeSingleTag Write a byte, short, int, long, float or double tag
   *
   * \tparam T Type of variable to be written
   *
   * \param type Type of nbt tag
   * \param Name The name of nbt tag
   * \param value The value of nbt tag
   * \return Bytes written.
   */
  template <typename T, bool convertToBE = true>
  int writeSingleTag(const tagType type, std::string_view Name, T value) {
    static_assert(std::is_trivial_v<T>);

    if (!this->is_open()) {
      return 0;
    }

    if constexpr (convertToBE) {
      value = convertLEBE(value);
    }

    int bytes = 0;
    if (Name.size() >= UINT16_MAX) {
      return 0;
    }
    const uint16_t realNameL = Name.size();
    const uint16_t flippedNameL = convertLEBE(realNameL);

    if (isInCompound()) {
      bytes += this->write_data(&type, sizeof(char));

      bytes += this->write_data(&flippedNameL, sizeof(flippedNameL));

      bytes += this->write_data(Name.data(), realNameL);

      bytes += this->write_data(&value, sizeof(T));

    } else {
      if (!typeMatch(type)) {
        return 0;
      }

      bytes += this->write_data(&value, sizeof(T));

      onElementWritten();
    }

    return bytes;
  }

  /**
   * \brief Write a byte tag
   * \param Name Name of tag
   * \param value Value of tag
   * \return Bytes written
   */
  inline int writeByte(std::string_view Name, int8_t value) {
    return writeSingleTag(tagType::Byte, Name, value);
  }

  /**
   * \brief Write a short tag
   * \param Name Name of tag
   * \param value Value of tag
   * \return Bytes written
   */
  inline int writeShort(std::string_view Name, int16_t value) {
    return writeSingleTag(tagType::Short, Name, value);
  }

  /**
   * \brief Write a int tag
   * \param Name Name of tag
   * \param value Value of tag
   * \return Bytes written
   */
  inline int writeInt(std::string_view Name, int32_t value) {
    return writeSingleTag(tagType::Int, Name, value);
  }

  /**
   * \brief Write a long tag
   * \param Name Name of tag
   * \param value Value of tag
   * \return Bytes written
   */
  inline int writeLong(std::string_view Name, int64_t value) {
    return writeSingleTag(tagType::Long, Name, value);
  }

  /**
   * \brief Write a float tag
   * \param Name Name of tag
   * \param value Value of tag
   * \return Bytes written
   */
  inline int writeFloat(std::string_view Name, float value) {
    return writeSingleTag(tagType::Float, Name, value);
  }

  /**
   * \brief Write a double tag
   * \param Name Name of tag
   * \param value Value of tag
   * \return Bytes written
   */
  inline int writeDouble(std::string_view Name, double value) {
    return writeSingleTag(tagType::Double, Name, value);
  }

  /**
   * \brief Write a compound tag
   * \param Name Name of tag
   * \return Bytes written
   */
  int writeCompound(std::string_view Name = "") {
    if (!this->is_open()) return 0;

    int bytes = 0;
    const int16_t realNameL = Name.size();
    const int16_t flippedNameL = convertLEBE(realNameL);

    if (isInCompound()) {
      bytes += this->write_data(&idCompound, sizeof(char));
      bytes += this->write_data(&flippedNameL, sizeof(int16_t));
      bytes += this->write_data(Name.data(), realNameL);

      tasks.emplace(task_t(End, 0));
      return bytes;
    }

    if (isInListOrArray() && typeMatch(Compound)) {
      // write nothing
      tasks.push(task_t(End, 0));
      return bytes;
    }

    return bytes;
  }

  /**
   * \brief Terminate current compound tag
   * \return Bytes written
   */
  int endCompound() {
    if (!this->is_open()) return 0;

    if (!isInCompound()) {
      return 0;
    }

    int bytes = 0;

    bytes += writeEnd();

    tasks.pop();

    if (isInListOrArray()) {
      onElementWritten();
    }

    return bytes;
  }

  /**
   * \brief Write the head of a list
   * \param Name Name of the list tag
   * \param elementType Type of list element
   * \param listSize Size of list
   * \return Bytes written
   */
  int writeListHead(std::string_view Name, tagType elementType,
                    const int listSize) {
    if (!this->is_open()) {
      return 0;
    }

    if (elementType == End) {
      return 0;
    }

    int bytes = 0;
    const int16_t realNameL = Name.size();
    const int16_t flippedNameL = convertLEBE(realNameL);
    const int32_t flippedListSize = convertLEBE<int32_t>(listSize);

    if (isInCompound()) {
      bytes += this->write_data(&idList, sizeof(char));
      bytes += this->write_data(&flippedNameL, sizeof(int16_t));
      bytes += this->write_data(Name.data(), realNameL);
      bytes += this->write_data(&elementType, sizeof(char));
      bytes += this->write_data(&flippedListSize, sizeof(int32_t));

      tasks.emplace(task_t(elementType, listSize));

      if (listSize == 0) {
        onElementWritten();
      }

      return bytes;
    }

    if (isInListOrArray() && typeMatch(List)) {
      bytes += this->write_data(&elementType, sizeof(char));
      bytes += this->write_data(&flippedListSize, sizeof(int32_t));

      tasks.emplace(task_t(elementType, listSize));
      if (listSize == 0) {
        onElementWritten();
      }

      return bytes;
    }
    return 0;
  }

 private:
  template <tagType elementType>
  int writeArrayHead(std::string_view Name, const int32_t arraySize) {
    if (!this->is_open()) {
      return 0;
    }

    static_assert((elementType == Byte) || (elementType == Int) ||
                  (elementType == Long));

    constexpr tagType arrayId =
        (elementType == Byte)
            ? (ByteArray)
            : ((elementType == Int) ? (IntArray) : (LongArray));

    int bytes = 0;
    const int16_t realNameL = Name.size();
    const int16_t flippedNameL = convertLEBE(realNameL);
    const int32_t flippedArraySize = convertLEBE<int32_t>(arraySize);

    if (isInCompound()) {
      bytes += this->write_data(&arrayId, sizeof(char));

      bytes += this->write_data(&flippedNameL, sizeof(int16_t));

      bytes += this->write_data(Name.data(), realNameL);

      bytes += this->write_data(&flippedArraySize, sizeof(int32_t));

      tasks.emplace(task_t(elementType, arraySize));

      if (arraySize == 0) {
        onElementWritten();
      }

      return bytes;
    }

    if (isInListOrArray() && typeMatch(arrayId)) {
      bytes += this->write_data(&flippedArraySize, sizeof(int32_t));

      tasks.emplace(task_t(elementType, arraySize));

      if (arraySize == 0) {
        onElementWritten();
      }

      return bytes;
    }

    return 0;
  }

 public:
  /**
   * \brief Start to write a byte array
   * \param Name Name of the array
   * \param arraySize Elements of the array
   * \return Bytes written
   */
  inline int writeByteArrayHead(std::string_view Name, const int arraySize) {
    return writeArrayHead<tagType::Byte>(Name, arraySize);
  }

  /**
   * \brief Start to write a int array
   * \param Name Name of the array
   * \param arraySize Elements of the array
   * \return Bytes written
   */
  inline int writeIntArrayHead(std::string_view Name, const int arraySize) {
    return writeArrayHead<tagType::Int>(Name, arraySize);
  }

  /**
   * \brief Start to write a long array
   * \param Name Name of the array
   * \param arraySize Elements of the array
   * \return Bytes written
   */
  inline int writeLongArrayHead(std::string_view Name, const int arraySize) {
    return writeArrayHead<tagType::Long>(Name, arraySize);
  }

  /**
   * \brief Write a string tag
   * \param Name Name of a string
   * \param value Value of a string
   * \return Bytes written
   */
  int writeString(std::string_view Name, const char *value) {
    if (!this->is_open()) {
      return 0;
    }

    int bytes = 0;

    const int16_t realNameL = Name.size();
    const int16_t flippedNameL = convertLEBE(realNameL);

    const int16_t realValueL = ::strlen(value);
    const int16_t flippedValueL = convertLEBE(realValueL);

    if (isInCompound()) {
      bytes += this->write_data(&idString, sizeof(char));
      bytes += this->write_data(&flippedNameL, sizeof(int16_t));
      bytes += this->write_data(Name.data(), realNameL);

      bytes += this->write_data(&flippedValueL, sizeof(int16_t));
      bytes += this->write_data(value, realValueL);

      return bytes;
    }

    if (isInListOrArray() && typeMatch(String)) {
      bytes += this->write_data(&flippedValueL, sizeof(int16_t));
      bytes += this->write_data(value, realValueL);

      onElementWritten();

      return bytes;
    }

    return 0;
  }

  /**
   * \brief Current type you can write into a list/array
   * \return Current type
   */
  inline tagType currentType() const { return tasks.top().currentTagType; }

  /**
   * \brief Bytes that have been written to the file.
   * \return Bytes written
   */
  inline size_t byteCount() const { return this->bytesWritten; }
};

};  // namespace NBT

#endif  //  SCL_NBTWRITER_H
