# 方块列表
这个文件夹存储了方块列表相关。

**FixedBlocks.json**是软件固定的方块列表，是最基础的那部分。每个方块相应的图片都存储在**FixedBlocks**文件夹下。正常情况下无需任何改动。

**CustomBlocks.json**是用户自定义的方块列表，允许用户灵活编辑。我在里面写了一些半砖方块，可以作为参考。**CustomBlocks**文件夹里有用户自定义方块的图片。

每个方块拥有以下属性：

| 属性名 | 类型 | 是否必填 | 默认值 | 说明 |
| :----: | :----: | :----: | :----: | ---- |
| baseColor | byte | 是 |  | 方块的地图基色 |
| id | string | 是 |  | 方块id |
| version | byte | 是 |  | 方块最早出现的版本。0代表1.12以前，12代表1.12，17代表1.17 |
| nameZH | string | 是 |  | 方块的中文名 |
| nameEN | string | 是 |  | 方块的英文名 |
| icon | string | 是 |  | 方块对应图片的文件名（仅文件名，如*cobblestone.png*） |
| idOld | string | 否 | 空字符串 | 方块在1.12的id |
| needGlass | bool | 否 | false | 指示方块底部是否必须有其他方块（如灯笼） |
| isGlowing | bool | 否 | false | 指示方块是否发光 |
| endermanPickable | bool | 否 | false | 指示方块是否可以被末影人偷走 |
| burnable | bool | 否 | false | 指示方块是否可以被烧毁 |

其中选填项可以跳过不填，SlopeCraft会自动补全为默认值。