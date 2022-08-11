/*
 Copyright © 2021-2022  TokiNoBug
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
    along with SlopeCraft.  If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/ToKiNoBug
    bilibili:https://space.bilibili.com/351429231
*/

#ifndef SLOPECRAFTL_NEWNBTWRITER_HPP
#define SLOPECRAFTL_NEWNBTWRITER_HPP

//#include <bits/endian>
#include <assert.h>
#include <stack>
#include <stdio.h>
#include <vector>
#include <stdint.h>
#include <string.h>
#include <type_traits>

//#include <iostream>

//using std::cout,std::endl;

namespace NBT {

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

template <typename T> inline T convertLEBE(T t) {

  uint8_t *ptr = reinterpret_cast<uint8_t *>(&t);

  for (int idx = 0; idx * 2 + 1 <= int(sizeof(T)); idx++) {
    std::swap(ptr[idx], ptr[sizeof(T) - idx - 1]);
  }

  return t;
}

/**
 * @brief The NBTWriter class
 */
class NBTWriter {
public:
  struct task_t {
    task_t() : currentTagType(tagType::End), taskSize(0) {}
    task_t(const tagType type, int size)
        : currentTagType(type), taskSize(size) {}

    tagType currentTagType;
    int taskSize;
  };

private:
  ::FILE *file;
  std::stack<task_t> tasks;
  size_t bytesWritten;

public:
  /**
   * @brief Default constructor
   */
  NBTWriter() : file(nullptr), tasks(), bytesWritten(0) {}
  NBTWriter(const char * fileName): file(nullptr), tasks(), bytesWritten(0) {
      open(fileName);
  }

  ~NBTWriter() {
    if (isOpen()) {
      close();
    }
  }

  bool open(const char *newFileName) {
    if (file != nullptr) {
      return false;
    }

    FILE *newfile = ::fopen(newFileName, "wb");

    if (newfile == NULL) {
      return false;
    }

    file = newfile;

    bytesWritten = 0;

    constexpr char head[3] = {10, 0, 0};

    writeData(head, sizeof(head));

    return true;
  }

  inline bool isOpen() const { return file != nullptr; }

  inline FILE *filePtr() { return file; }

  inline const FILE *filePtr() const { return file; }

  bool close() {
      // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      
    if (file == nullptr) {
      return false;
    }
    // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;

    if (!tasks.empty())
      emergencyFill();
    // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;

    constexpr char fileTail[1] = {idEnd};

    writeData(fileTail, sizeof(fileTail));
    // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;

    fclose(file);
    // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;

    file = nullptr;
    // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
    return true;
  }

  int emergencyFill() {

      // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      
    if (tasks.empty()) {
      return 0;
    }
    // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;

    int bytes = 0;

    // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
    while (!tasks.empty()) {
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      
      if (isInCompound()) {
        bytes += endCompound();
        continue;
      }
      // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;

      cout<<"tasks.size() = "<<tasks.size()<<endl;

      switch (currentType()) {
      case End:
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
        exit(114514);
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      
        continue;
      case Byte:
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
        bytes += writeByte("autoByte", 114);
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      
        continue;
      case Short:
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
        bytes += writeShort("autoShort", 514);
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      
        continue;
      case Int:
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
        bytes += writeInt("autoInt", 114514);
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      
        continue;
      case Long:
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
        bytes += writeLong("autoLong", 1919810);
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      
        continue;
      case Float:
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
        bytes += writeFloat("autoFloat", 114.514);
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      
        continue;
      case Double:
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
        bytes += writeDouble("autoDouble", 1919810.114514);
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      
        continue;
      case String:
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
        bytes += writeString("autoString", "FuckYou!");
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      
        continue;
      case List:
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
        bytes += writeListHead("autoList", Int, 1);
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      
        continue;
      case Compound:
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
        bytes += writeCompound("autoCompound");
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      
        continue;
      case ByteArray:
        bytes += writeByteArrayHead("autoByteArray", 1);
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      
        continue;
      case IntArray:
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
        bytes += writeIntArrayHead("autoIntArray", 1);
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      
        continue;
      case LongArray:
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
        bytes += writeLongArrayHead("autoLongArray", 1);
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      
        continue;
      }
      // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      
    }

    // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
    bytes += writeString("TokiNoBug'sWarning",
                         "There's sth wrong with ur NBTWriter, the file format "
                         "is completed automatically instead of manually.");

    // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
    return bytes;
  }

private:
  int writeData(const void *data, const size_t bytes) {
    ::fwrite(data, sizeof(char), bytes, file);

    bytesWritten += bytes;

    return bytes;
  }

public:
  inline bool isInCompound() const {
    return tasks.empty() || (tasks.top().currentTagType == tagType::End);
  }

  inline bool isInListOrArray() const { return !isInCompound(); }

  inline bool typeMatch(const tagType type) const {
    return type == tasks.top().currentTagType;
  }

  inline bool isListOrArrayFinished() const {
    return tasks.top().taskSize <= 0;
  }

private:
  inline void onElementWritten() {

    if(!isInListOrArray()) {
      return;
    }

    if(!isListOrArrayFinished()) {
      tasks.top().taskSize--;
    }

    tryEndList();
  }


  inline void tryEndList() {


    if(!isInListOrArray()) {
      return;
    }

    while(!tasks.empty()) {
    cout<<"stack size = "<<tasks.size()<<endl;
      if(!isInListOrArray()) {
        return;
      }

      if(isListOrArrayFinished()) {
        tasks.pop();
        continue;
      }
      else {
        return;
      }
    }
  }
  

  inline int writeEnd() {
    writeData(&idEnd, 1);
    return 1;
  }

public:
  template <typename T, bool convertToBE = true>
  int writeSingleTag(const tagType type, const char *Name, T value) {
    static_assert(std::is_trivial_v<T>);

        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
    if (!isOpen()) {
      return 0;
    }

        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
    if constexpr (convertToBE) {
      value = convertLEBE(value);
    }

        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
    int bytes = 0;
    const uint16_t realNameL = strlen(Name);
    const uint16_t flippedNameL = convertLEBE(realNameL);

        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
    if (isInCompound()) {
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      bytes += writeData(&type, sizeof(char));
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      bytes += writeData(&flippedNameL, sizeof(flippedNameL));
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      bytes += writeData(Name, realNameL);
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      bytes += writeData(&value, sizeof(T));
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
    } else {
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      if (!typeMatch(type)) {
        return 0;
      }
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;

      bytes += writeData(&value, sizeof(T));
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      onElementWritten();
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
    }
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;

    return bytes;
  }

  inline int writeByte(const char *Name, int8_t value) {
    return writeSingleTag(tagType::Byte, Name, value);
  }

  inline int writeShort(const char *Name, int16_t value) {
    return writeSingleTag(tagType::Short, Name, value);
  }

  inline int writeInt(const char *Name, int32_t value) {
    return writeSingleTag(tagType::Int, Name, value);
  }

  inline int writeLong(const char *Name, int64_t value) {
    return writeSingleTag(tagType::Long, Name, value);
  }

  inline int writeFloat(const char *Name, float value) {
    return writeSingleTag(tagType::Float, Name, value);
  }

  inline int writeDouble(const char *Name, double value) {
    return writeSingleTag(tagType::Double, Name, value);
  }

  int writeCompound(const char *Name = "") {
    if (!isOpen())
      return 0;

    int bytes = 0;
    const int16_t realNameL = ::strlen(Name);
    const int16_t flippedNameL = convertLEBE(realNameL);

    if (isInCompound()) {
      bytes += writeData(&idCompound, sizeof(char));
      bytes += writeData(&flippedNameL, sizeof(int16_t));
      bytes += writeData(Name, realNameL);

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

  int endCompound() {
    if (!isOpen())
      return 0;

    if (!isInCompound()) {
      return 0;
    }

    int bytes = 0;

    bytes += writeEnd();

    tasks.pop();

    if(isInListOrArray()) {
        onElementWritten();
    }

    return bytes;
  }

  int writeListHead(const char *Name, tagType elementType, const int listSize) {
    if (!isOpen()) {
      return 0;
    }

    if (elementType == End) {
      return 0;
    }

    int bytes = 0;
    const int16_t realNameL = ::strlen(Name);
    const int16_t flippedNameL = convertLEBE(realNameL);
    const int32_t flippedListSize = convertLEBE<int32_t>(listSize);

    if (isInCompound()) {
      bytes += writeData(&idList, sizeof(char));
      bytes += writeData(&flippedNameL, sizeof(int16_t));
      bytes += writeData(Name, realNameL);
      bytes += writeData(&elementType, sizeof(char));
      bytes += writeData(&flippedListSize, sizeof(int32_t));

      tasks.emplace(task_t(elementType, listSize));

      if (listSize == 0) {
        onElementWritten();
      }

      return bytes;
    }

    if (isInListOrArray() && typeMatch(List)) {
      bytes += writeData(&elementType, sizeof(char));
      bytes += writeData(&flippedListSize, sizeof(int32_t));

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
  int writeArrayHead(const char *Name, const int32_t arraySize) {
      // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      
    if (!isOpen()) {
      return 0;
    }
    // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;

    static_assert((elementType == Byte) || (elementType == Int) ||
                  (elementType == Long));

    constexpr tagType arrayId =
        (elementType == Byte)
            ? (ByteArray)
            : ((elementType == Int) ? (IntArray) : (LongArray));

    int bytes = 0;
    const int16_t realNameL = ::strlen(Name);
    const int16_t flippedNameL = convertLEBE(realNameL);
    const int32_t flippedArraySize = convertLEBE<int32_t>(arraySize);

    // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
    if (isInCompound()) {
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      
      bytes += writeData(&arrayId, sizeof(char));
      // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      
      bytes += writeData(&flippedNameL, sizeof(int16_t));
      // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      
      bytes += writeData(Name, realNameL);
      // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      

      bytes += writeData(&flippedArraySize, sizeof(int32_t));
      // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      

      tasks.emplace(task_t(elementType, arraySize));
      // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      

      if (arraySize == 0) {
          // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      
        onElementWritten();
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      
      }
      // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      

      return bytes;
    }
    // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;

    if (isInListOrArray() && typeMatch(arrayId)) {
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      
      bytes += writeData(&flippedArraySize, sizeof(int32_t));

      // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      
      tasks.emplace(task_t(elementType, arraySize));
      // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      

      if (arraySize == 0) {
          // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      
        onElementWritten();
        // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      
      }

      // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;
      
      return bytes;
    }
    // printf("%s , %d\n",__FILE__,__LINE__);cout<<endl;

    return 0;
  }

public:
  inline int writeByteArrayHead(const char *Name, const int arraySize) {
    return writeArrayHead<tagType::Byte>(Name, arraySize);
  }

  inline int writeIntArrayHead(const char *Name, const int arraySize) {
    return writeArrayHead<tagType::Int>(Name, arraySize);
  }

  inline int writeLongArrayHead(const char *Name, const int arraySize) {
    return writeArrayHead<tagType::Long>(Name, arraySize);
  }

  int writeString(const char *Name, const char *value) {
    if (!isOpen()) {
      return 0;
    }

    int bytes = 0;

    const int16_t realNameL = ::strlen(Name);
    const int16_t flippedNameL = convertLEBE(realNameL);

    const int16_t realValueL = ::strlen(value);
    const int16_t flippedValueL = convertLEBE(realValueL);

    if (isInCompound()) {
      bytes += writeData(&idString, sizeof(char));
      bytes += writeData(&flippedNameL, sizeof(int16_t));
      bytes += writeData(Name, realNameL);

      bytes += writeData(&flippedValueL, sizeof(int16_t));
      bytes += writeData(value, realValueL);

      return bytes;
    }

    if (isInListOrArray() && typeMatch(String)) {

      bytes += writeData(&flippedValueL, sizeof(int16_t));
      bytes += writeData(value, realValueL);

      onElementWritten();

      return bytes;
    }

    return 0;
  }

  inline tagType currentType() const { return tasks.top().currentTagType; }

  inline size_t byteCount() const { return bytesWritten; }
};

}; // namespace NBTWriter

#endif //  SLOPECRAFTL_NEWNBTWRITER_HPP
