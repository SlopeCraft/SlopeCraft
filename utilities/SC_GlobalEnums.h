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

#ifndef SCL_GLOBALENUMS_GLOBALENUMERATIONS_H
#define SCL_GLOBALENUMS_GLOBALENUMERATIONS_H

/**
 * \brief This file defines enumerations for all executable and libraries.
 *
 */

/**
 * @brief Minecraft game version
 */
enum SCL_gameVersion {
  /// older than 1.12
  ANCIENT = 0,
  /// 1.12
  MC12 = 12,
  /// 1.13
  MC13 = 13,
  /// 1.14
  MC14 = 14,
  /// 1.15
  MC15 = 15,
  /// 1.16
  MC16 = 16,
  /// 1.17
  MC17 = 17,
  /// 1.18
  MC18 = 18,
  /// 1.19
  MC19 = 19,
  /// future version
  FUTURE = 255
};

const SCL_gameVersion max_version = SCL_gameVersion::MC19;

/// color difference formula used to match colors
enum SCL_convertAlgo : char {
  /// naive RGB
  RGB = 'r',
  /// RGB with rotation
  RGB_Better = 'R',
  /// naive HSV formula
  HSV = 'H',
  /// CIELAB 1994 formula
  Lab94 = 'l',
  /// CIELAB 2000 formula
  Lab00 = 'L',
  /// naive XYZ formula
  XYZ = 'X',
  /// Genetic Algorithm
  gaCvter = 'A'
};

enum SCL_compressSettings : int {
  /// don't compress
  noCompress = 0,
  /// compress in lossless only
  NaturalOnly = 1,
  /// compress in lossy only
  ForcedOnly = 2,
  /// compress with both lossless and lossy
  Both = 3
};

enum SCL_glassBridgeSettings : int {
  /// don't construce bridge
  noBridge = 0,
  /// construct bridge
  withBridge = 1
};

enum SCL_mapTypes : int {
  /// 3D
  Slope = 0,
  /// flat
  Flat = 1,
  /// map data files
  FileOnly = 2
};

enum SCL_step : int {
  /// the instance is created
  nothing,
  /// map type is set and waitting for image
  wait4Image,
  /// image is ready and ready for converting
  convertionReady,
  /// image is converted and ready for building 3D structure, exporting as
  /// file-only map(s) can be done in this step
  converted,
  /// 3D structure is built and ready for exporting 3d structure
  builded,
};

enum SCL_errorFlag : int {
  /// no error
  NO_ERROR_OCCUR = -1,
  /// trying to skip steps
  HASTY_MANIPULATION = 0x00,
  /// failed when compressing in lossy
  LOSSYCOMPRESS_FAILED = 0x01,
  /// color in shadow 3 appears in vanilla map
  DEPTH_3_IN_VANILLA_MAP = 0x02,
  ///
  MAX_ALLOWED_HEIGHT_LESS_THAN_14 = 0x03,
  /// too few color to convert
  USEABLE_COLOR_TOO_FEW = 0x04,
  /// the original image is empty
  EMPTY_RAW_IMAGE = 0x05,
  /// failed to gzip
  FAILED_TO_COMPRESS = 0x06,
  /// failed to remove uncompressed files
  FAILED_TO_REMOVE = 0x07,
  /// Wrong file extension name when exporitng a schem
  EXPORT_SCHEM_WRONG_EXTENSION = 0x08,
  /// Failed to create or open a file stream when exporing a schem
  EXPORT_SCHEM_FAILED_TO_CREATE_FILE = 0x09,
  /// libSchem found invalid blocks when exporting a schem
  EXPORT_SCHEM_HAS_INVALID_BLOCKS = 0x0A,
  /// The size of block palette exceeds the maximum size(usually 32766,or
  /// 65535). I don't believe this error can happen.
  EXPORT_SCHEM_BLOCK_PALETTE_OVERFLOW = 0x0B,
  /// This error only happens when you exports as structure AND set
  /// is_air_structure_void to false and you don't have minecraft:air in the
  /// palette
  EXPORT_SCHEM_STRUCTURE_REQUIRES_AIR = 0x0C,
  /// the major game version is less that MC12 or greater than MC19
  UNKNOWN_MAJOR_GAME_VERSION = 0x0D,
  /// Exporting as WESchem doesn't support MC12, since the format of schematic
  /// is greatly different.
  EXPORT_SCHEM_MC12_NOT_SUPPORTED = 0x0E,

};

enum SCL_workStatues : int {
  /// waiting
  none = -1,

  collectingColors = 0x00,
  converting = 0x01,
  dithering = 0x02,
  // convertFinished=0x03,

  buidingHeighMap = 0x10,
  compressing = 0x11,
  building3D = 0x12,
  constructingBridges = 0x13,
  flippingToWall = 0x14,

  writingMetaInfo = 0x20,
  writingBlockPalette = 0x21,
  writing3D = 0x22,
  // slopeFinished=0x16,

  writingMapDataFiles = 0x30,
  // dataFilesFinished=0x31,
};

/**
 * @brief Pure transparent pixel processing strategy of SCL
 */
enum SCL_PureTpPixelSt : char {
  ReplaceWithBackGround = 'B',
  ReserveAsAir = 'A'
};

/**
 * @brief The SCL_HalfTpPixelSt enum
 */
enum SCL_HalfTpPixelSt : char {
  ReplaceWithBackGround_ = 'B',
  ComposeWithBackGround = 'C',
  IgnoreAlpha = 'R'
};

#ifdef __cplusplus
#define VCL_CLASS_IF_CPP class
#else
#define VCL_CLASS_IF_CPP
#endif // #ifdef __cplusplus

enum VCL_CLASS_IF_CPP VCL_Kernel_step : int {
  VCL_none = 0,
  VCL_wait_for_image = 1,
  VCL_converted = 2,
  VCL_built = 3
};

#endif