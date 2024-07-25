find_package(libnbt++ REQUIRED)

add_library(sNBT-formatter STATIC sNBT_formatter.h sNBT_formatter.cpp)
target_link_libraries(sNBT-formatter PUBLIC nbt++)