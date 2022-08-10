# SlopeCraft FAQ

<br>

**简体中文** | [English](FAQ_EN.md "FAQ_EN.md")  <!-- lang -->

<br>


## 发现bug之前，请先试一下最新版本有没有修复这个bug，谢谢！

1. 基岩版/网易版能不能用？
   - 我从来不玩这些版本，没有研究过，不知道能不能用，不知道怎么用。
   - **理论上**无论是国际版java还是网易java，只要是java版且成功安装了litematica mod，那么就可以使用。
   - 基岩版目前没有litematica mod，所以**理论上**不可以使用。

2. 想在服务器里造，但服务端没装**投影mod**
   - 首先强调，本软件的界面及一切文档中，投影mod一律指**Litematica mod**，开发者为Masa。其他任何含“投影”字样或类似功能的mod，都不是本软件需要的。
   - Litematica mod是**客户端mod**，**不需要服务端安装**它。只需要客户端安装了Litematica和它依赖的前置mod Malilib，即可正常使用投影mod。所以不要再问这种蠢问题了。
   - [Litematica Mod - CurseForge](https://www.curseforge.com/minecraft/mc-mods/litematica)
   - [Litematica Mod - MC百科](https://www.mcmod.cn/class/2261.html)

3. SlopeCraft.exe 是在文件里的哪个地方？
   - 不！要！下！载！源！代！码！！！！！！！
   - 实在不会用 Github 可以直接在蓝奏云/度盘下载
   - github下载教程：在repo的主界面(main)，在网页右边有一个release的地方，点击之后就能看到用户可以使用的下载方式了

4. **.7z**文件是什么？怎么打开？
   - （是的没错真的收到了这样的问题）
   - 自！己！百！度！
   - winrar或者bandizip都行。

5. 该怎么制作非方形的地图画？
   - 为什么有人会认为SlopeCraft不能制作非方形的呢？
   - SlopeCraft**从来不会缩放你的图片**，而是忠实的还原每一个像素。**你可以制作任何尺寸的地图画，照常处理就行**。

6. SlopeCraft 点了调整颜色之后图片几乎是灰的？/为什么颜色不理想/……
   - 有哪几种颜色是 Mojang 钦定的，有一些颜色就是没有特别相似的地图色，比如浅蓝、灰蓝、浅粉、浅紫，只能寻找差别不是太大的颜色。
   - v3.5已经实现了抖动/仿色算法，它可以解决这个问题。

7. `/give` 命令得不到地图物品怎么办？
   - 1.12使用 `/give @p filled_map 1 i`，获得序号为i的地图
   - 1.13+使用 `/give @p filled_map{map:i}`，获得序号为i的地图

8. 可不可以把地图缩小？
   - 不建议，因为无论缩小与否，地图的分辨率仍然是128*128像素，画质只会降低。提高分辨率的最好方式还是用多张无缩放的地图组合。

9.  导出的投影文件不能正常读取？
   - 最好不要在**投影区域名称**中写中文，否则可能会因为汉字编码格式问题而导致乱码。
