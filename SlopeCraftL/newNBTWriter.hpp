#ifndef SLOPECRAFTL_NEWNBTWRITER_HPP
#define SLOPECRAFTL_NEWNBTWRITER_HPP

//#include <bits/endian>
#include <assert.h>
#include <stack>
#include <stdio.h>
#include <vector>

#include <type_traits>

namespace NBTWriter {

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
  std::FILE *file;
  std::stack<task_t> tasks;
  size_t bytesWritten;

public:
  NBTWriter() : file(nullptr), tasks(), bytesWritten(0) {}

  ~NBTWriter() {
    if (isOpen()) {
      close();
    }
  }

  bool open(const char *newFileName) {
    if (file != nullptr) {
      return false;
    }

    FILE *newfile = std::fopen(newFileName, "wb");

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
    if (file == nullptr) {
      return false;
    }

    if (!tasks.empty())
      emergencyFill();

    constexpr char fileTail[1] = {idEnd};

    writeData(fileTail, sizeof(fileTail));

    fclose(file);

    file = nullptr;
    return true;
  }

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
  int writeData(const void *data, const size_t bytes) {
    std::fwrite(data, sizeof(char), bytes, file);

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
    if (isInListOrArray() && !isListOrArrayFinished()) {
      tasks.top().taskSize--;
    }

    if (isListOrArrayFinished()) {
      endList();
    }
  }

  inline void endList() {

    if (isInListOrArray() && isListOrArrayFinished()) {
      tasks.pop();
      onElementWritten();
    }
  }

  inline int writeEnd() {
    writeData(&idEnd, 1);
    return 1;
  }

  template <typename T, bool convertToBE = true>
  int writeSingleTag(const tagType type, const char *Name, T value) {
    static_assert(std::is_trivial_v<T>);

    if (!isOpen()) {
      return 0;
    }

    if constexpr (convertToBE) {
      value = convertLEBE(value);
    }

    int bytes = 0;
    const uint16_t realNameL = strlen(Name);
    const uint16_t flippedNameL = convertLEBE(realNameL);

    if (isInCompound()) {
      bytes += writeData(&type, sizeof(char));
      bytes += writeData(&flippedNameL, sizeof(flippedNameL));
      bytes += writeData(Name, realNameL);
      bytes += writeData(&value, sizeof(T));
    } else {
      if (!typeMatch(type)) {
        return 0;
      }

      bytes += writeData(&value, sizeof(T));
      onElementWritten();
    }

    return bytes;
  }

public:
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
    const int16_t realNameL = strlen(Name);
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
    const int16_t realNameL = std::strlen(Name);
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
    if (!isOpen()) {
      return 0;
    }

    static_assert((elementType == Byte) || (elementType == Int) ||
                  (elementType == Long));

    constexpr tagType arrayId =
        (elementType == Byte)
            ? (ByteArray)
            : ((elementType == Int) ? (IntArray) : (LongArray));

    int bytes = 0;
    const int16_t realNameL = std::strlen(Name);
    const int16_t flippedNameL = convertLEBE(realNameL);
    const int32_t flippedArraySize = convertLEBE<int32_t>(arraySize);

    if (isInCompound()) {
      bytes += writeData(&arrayId, sizeof(char));
      bytes += writeData(&flippedNameL, sizeof(int16_t));
      bytes += writeData(Name, realNameL);

      bytes += writeData(&flippedArraySize, sizeof(int32_t));

      tasks.emplace(task_t(Byte, arraySize));

      if (arraySize == 0) {
        onElementWritten();
      }

      return bytes;
    }

    if (isInListOrArray() && typeMatch(ByteArray)) {
      bytes += writeData(&flippedArraySize, sizeof(int32_t));

      tasks.emplace(task_t(Byte, arraySize));

      if (arraySize == 0) {
        onElementWritten();
      }

      return bytes;
    }

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

    const int16_t realNameL = std::strlen(Name);
    const int16_t flippedNameL = convertLEBE(realNameL);

    const int16_t realValueL = std::strlen(value);
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