# SlopeCraft FAQ

## 发现bug之前，请先试一些最新版本有没有修复这个bug，谢谢

1. 基岩版/网易版能不能用？
   - 现在不能，因为没有找到基岩版的类似于投影mod/World Edit的mod。
   - 如果你知道基岩版有合适的导入方式，请告诉我。

2. SlopeCraft.exe 是在文件里的哪个地方？
   - 不！要！下！载！源！代！码！
   - 实在不会用 Github 可以直接在蓝奏云/度盘下载
   - github下载教程：在repo的主界面(main)，在网页右边有一个release的地方，点击之后就能看到用户可以使用的下载方式了

3. SlopeCraft 点了调整颜色之后图片几乎是灰的？/为什么颜色不理想/……
   - 有哪几种颜色是 Mojang 钦定的，有一些颜色就是没有特别相似的地图色，比如浅蓝、灰蓝、浅粉、浅紫，只能寻找差别不是太大的颜色。
   - 已经实现了抖动/仿色算法。

4. `/give` 命令得不到地图物品怎么办？
   - 1.12使用 `/give @p filled_map 1 i`，获得序号为i的地图
   - 1.13+使用 `/give @p filled_map{map:i}`，获得序号为i的地图

5. 可不可以把地图缩小？
   - 不建议，因为无论缩小与否，地图的分辨率仍然是128*128像素，画质只会降低。提高分辨率的最好方式还是用多张无缩放的地图组合。

6. 导出的投影文件不能正常读取？
   - 最好不要在**投影区域名称**中写中文，否则可能会因为汉字编码格式问题而导致乱码。

## Before submitting an issue for a bug, make sure the bug isn't fixed in the latest version. Thank you!

1. Can this be used in Bedrock edition?
   - Not at the moment - no mod that resembles World Edit or Litematic was found.
   - If you know any ways of importing the files from this program into Bedrock Edition, please tell me.

2. SlopeCraft.exe isn't in this zip file
   - DO NOT DOWNLOAD THE SOURCE CODE
   - If you can't use github properly, use any netdisk provided
   - A quick tutorial on github: in the repo main screen, there should be a link on the right side to Release, click into it, then download the latest stable version

3. SlopeCraft turned everything grey after I clicked color adjust? / Why does the color look nothing like the original?
 - Some colors are featured in game, while some others aren't, such as light blue, greyish blue, light pink, light purple, so a substitute color is required
 - A color simulation feature has been implemented

4. '/give' didn't give me the map item
 - 1.12 Command: `/give @p filled_map 1 i` where i is the number for the map
 - 1.13+ Command: `/give @p filled_map{map:i}` where i is the number for the map

5. Can I shrink the map? 
 - I don't recommend it. Whether or not the map is shrinked, the pixel will always be 128x128, and the quality will be decreased, the best way to increase quality is still using multiple maps

6. My Litematic file wasn't able to be read
 - Don't put non-English characters into the **Litematic File Name** area, it will screw it up.
