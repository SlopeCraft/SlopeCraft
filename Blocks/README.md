# 方块列表

这个压缩包存储了方块列表相关，包含`block_list.json`和若干方块对应的图片（必须为PNG格式）。

**FixedBlocks.zip**是软件固定的方块列表，是最基础的那部分。每个方块相应的图片都存储在**FixedBlocks**文件夹下。正常情况下无需任何改动。

**CustomBlocks.zip**是用户自定义的方块列表，允许用户灵活编辑。我在里面写了一些半砖方块，可以作为参考。**CustomBlocks**
文件夹里有用户自定义方块的图片。

`block_list.json`记录了所有方块，每个方块拥有以下属性：

|       属性名        |   类型   | 是否必填 |  默认值  | 说明                                                |
|:----------------:|:------:|:----:|:-----:|---------------------------------------------------|
|    baseColor     |  byte  |  是   |       | 方块的地图基色                                           |
|        id        | string |  是   |       | 方块 id，附上详细方块状态，如*minecraft:target[power=0]*       |
|     version      |  byte  |  是   |       | 方块最早出现的版本。0 代表 1.12 以前，12 代表 1.12，17 代表 1.17，以此类推 |
|      nameZH      | string |  是   |       | 方块的中文名                                            |
|      nameEN      | string |  是   |       | 方块的英文名                                            |
|       icon       | string |  是   |       | 方块对应图片的文件名（仅文件名，如*cobblestone.png*）               |
|      idOld       | string |  否   | 空字符串  | 方块在 1.12 的 id                                     |
|    needGlass     |  bool  |  否   | false | 方块底部是否必须有其他方块（如灯笼）                                |
|    isGlowing     |  bool  |  否   | false | 是否发光                                              |
| endermanPickable |  bool  |  否   | false | 是否可以被末影人偷走                                        |
|     burnable     |  bool  |  否   | false | 是否可以被烧毁                                           |

其中选填项可以跳过不填，SlopeCraft 会自动补全为默认值。

<br>
<br>

# block_list

Everything about blocklist is in this zip, including `block_list.json` and block images (must be PNG format).

**FixedBlocks.zip** stores all default blocks, acting as a fundamental blocklist to make sure that each
basecolor_widgets has at least one block. Image of each block is stored in directory named **FixedBlocks**. Usually you
don't need to change anything about fixed blocklist.

**CustomBlocks.zip** stores all user custom blocks, enabling users to use their favorite blocks in map art. Some slab
blocks have been already written in this file as examples. Each block should have a corresponding image in *
*CustomBlocks** directory. Different blocks can share single image file.

Each block has the following attributes:

|       Name       |  Type  | Is Compulsory Item | Default Value | Description                                                                                                |
|:----------------:|:------:|:------------------:|:-------------:|:-----------------------------------------------------------------------------------------------------------|
|    baseColor     |  byte  |        Yes         |               | The base color of this block                                                                               |
|        id        | string |        Yes         |               | Block id in minecraft with full blockstates, like *minecraft:target[power=0]*                              |
|     version      |  byte  |        Yes         |               | The earlist version when block is added. 0 means earlier than 1.12, 12 means 1.12, 17 means 1.17 and so on |
|      nameZH      | string |        Yes         |               | Chinese name                                                                                               |
|      nameEN      | string |        Yes         |               | English name                                                                                               |
|       icon       | string |        Yes         |               | File name of image ( file name only, like *cobblestone.png*)                                               |
|      idOld       | string |         No         | Empty string  | Block id in 1.12                                                                                           |
|    needGlass     |  bool  |         No         |     false     | Whether a full block is required under this block (like lantern)                                           |
|    isGlowing     |  bool  |         No         |     false     | Whether a block emits light                                                                                |
| endermanPickable |  bool  |         No         |     false     | Whether a block could be stolen by enderman                                                                |

Uncompulsory items can be skipped, SlopeCraft will set them as their default value.