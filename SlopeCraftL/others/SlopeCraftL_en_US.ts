<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="en_US">
<context>
    <name>SlopeCraft::SCLTranslator</name>
    <message>
        <location filename="../color_table.cpp" line="121"/>
        <source>SlopeCraftL可用颜色过少：仅仅%1种。
可用的基色：%2</source>
        <translation>Too few colors for SlopeCraftL: only %1 color(s).
Avaibale base colors: %2</translation>
    </message>
    <message>
        <location filename="../color_table.cpp" line="332"/>
        <location filename="../color_table.cpp" line="338"/>
        <source>导出结构方块文件%1失败，：%2</source>
        <translation>Failed to export vanilla structure file %1 due to: %2</translation>
    </message>
    <message>
        <source>无法将缓存保存至文件 %1：%2</source>
        <translation type="vanished">Failed to save cache to file &quot;%1&quot;: %2</translation>
    </message>
    <message>
        <source>异常：%1</source>
        <translation type="vanished">Caught exception: %1</translation>
    </message>
    <message>
        <location filename="../color_table.cpp" line="251"/>
        <source>文件名必须以 *.nbt 结尾</source>
        <translation>File name should end with &quot;.nbt&quot;</translation>
    </message>
    <message>
        <source>导出结构方块文件%1失败，错误码%2，详情：%3</source>
        <translation type="vanished">Failed to export structure file %1, error code = %2, detail: %3</translation>
    </message>
    <message>
        <location filename="../color_table.cpp" line="354"/>
        <source>遇到无效方块id &quot;%1&quot;</source>
        <translation>Found invalid block id &quot;%1&quot;</translation>
    </message>
    <message>
        <location filename="../SlopeCraftL.cpp" line="67"/>
        <source>函数SCL_create_block_list_from_buffer遇到无效的输入，缓冲区为空</source>
        <translation>SCL_create_block_list_from_buffer met invalid value, either buffer is nullptr or buffer size is 0</translation>
    </message>
    <message>
        <location filename="../structure_3D.cpp" line="28"/>
        <source>允许最大高度应该不低于14，但是遇到了%1</source>
        <translation>Max allowed height should be &gt;= 14, but found %1</translation>
    </message>
    <message>
        <location filename="../structure_3D.cpp" line="49"/>
        <source>创建区域失败：%1</source>
        <translation>Failed to create region: %1</translation>
    </message>
    <message>
        <location filename="../structure_3D.cpp" line="110"/>
        <source>分配内存失败，需要%1GiB。详情：&quot;%2&quot;</source>
        <translation>Failed to allocate memory for this structure, required %1 GiB. The exception says: &quot;%2&quot;</translation>
    </message>
    <message>
        <location filename="../structure_3D.cpp" line="298"/>
        <source>SlopeCraftL精简方块表时遇到内部错误：%1</source>
        <translation>Internal error when SlopeCraftL tries to shrink palette: %1</translation>
    </message>
    <message>
        <location filename="../structure_3D.cpp" line="353"/>
        <source>SlopeCraftL 内部错误：%1</source>
        <translation>SlopeCraft internal error: %1</translation>
    </message>
    <message>
        <location filename="../structure_3D.cpp" line="375"/>
        <source>SlopeCraftL 内部错误：mc_schem不支持数据版本%1</source>
        <translation>SlopeCraft internal error: mc_schem doesn&apos;t support data version %1</translation>
    </message>
    <message>
        <location filename="../structure_3D.cpp" line="411"/>
        <location filename="../structure_3D.cpp" line="437"/>
        <location filename="../structure_3D.cpp" line="468"/>
        <source>导出%1失败：%2</source>
        <translation>Failed to export %1 because: %2</translation>
    </message>
    <message>
        <location filename="../structure_3D.cpp" line="488"/>
        <source>SlopeCraftL只能把平面地图画导出为平面示意图，但是遇到了%1</source>
        <translation>SlopeCraftL can only export flat diagram for flat maps, but found %1</translation>
    </message>
    <message>
        <location filename="../color_table.cpp" line="294"/>
        <location filename="../structure_3D.cpp" line="519"/>
        <source>SlopeCraftL内部错误：%1</source>
        <translation>SlopeCraftL internal error: %1</translation>
    </message>
    <message>
        <location filename="../structure_3D.cpp" line="548"/>
        <source>SlopeCraftL内部错误。无法找到与方块%1匹配的图像。3D结构中对应的方块索引为%2。整个方块列表如下：%3</source>
        <translation>SlopeCraftL internal error. Failed to find block image &quot;for \&quot;%1\&quot;. In the 3d structure, the corresponding block idx is %2.\nThe whole block palette is as below: %3</translation>
    </message>
    <message>
        <source>序列化时遇到负尺寸：%1行%2列</source>
        <translation type="vanished">Found negative shape when deserializing: %1 rows and %2 cols</translation>
    </message>
    <message>
        <location filename="../converted_image.cpp" line="198"/>
        <source>无法创建nbt文件 %1</source>
        <translation>Failed to create nbt file %1</translation>
    </message>
    <message>
        <location filename="../converted_image.cpp" line="111"/>
        <location filename="../converted_image.cpp" line="158"/>
        <source>无效的游戏版本 %1</source>
        <translation>Wrong game version %1</translation>
    </message>
    <message>
        <location filename="../converted_image.cpp" line="251"/>
        <source>SlopeCraftL内部错误：在原版地图画中发现阴影&gt;=3的的地图色。地图画内容（地图色，列优先）：
%1</source>
        <translation>SlopeCraftL internal error: found mapcolor with shadow &gt;=3 in vanilla maps. Map contents (mapcolor, col-major): %1</translation>
    </message>
    <message>
        <location filename="../converted_image.cpp" line="351"/>
        <source>3D结构的第%1列压缩失败。要求最大高度&lt;=%2，但是只能压缩到%3</source>
        <translation>Failed to compress the 3D structure at column %1. You have required that max height ≤ %2 , but SlopeCraft is only able to compress this column to max height = %3</translation>
    </message>
    <message>
        <location filename="../converted_image.cpp" line="769"/>
        <source>内部错误：mc_schem重新解析nbt失败：%1</source>
        <translation>Internal error: mc_schem failed to parse NBT: %1</translation>
    </message>
    <message>
        <location filename="../converted_image.cpp" line="776"/>
        <source>内部错误：mc_schem重新解析实体失败：%1</source>
        <translation>Internal error: mc_schem failed to reparse entity: %1</translation>
    </message>
    <message>
        <location filename="../converted_image.cpp" line="791"/>
        <source>内部错误：mc_schem不支持数据版本%1</source>
        <translation>Internal error: mc_schem doesn&apos;t support data version %1</translation>
    </message>
    <message>
        <source>打开文件%1失败</source>
        <translation type="vanished">Failed to open file %1</translation>
    </message>
    <message>
        <source>文件%1不存在</source>
        <translation type="vanished">File %1 doesn&apos;t exist</translation>
    </message>
    <message>
        <source>加载缓存失败，%1包含错误</source>
        <translation type="vanished">Failed to load cache from %1, the cache is incorrect</translation>
    </message>
    <message>
        <location filename="../converted_image.cpp" line="502"/>
        <source>错误的地图尺寸：%1行，%2列</source>
        <translation>Invalid map size: %1 rows, %2 cols</translation>
    </message>
    <message>
        <location filename="../block_list.cpp" line="57"/>
        <source>无效的方块基色 %1</source>
        <translation>Invalid base color: %1</translation>
    </message>
    <message>
        <location filename="../block_list.cpp" line="71"/>
        <source>无效的方块版本 %1</source>
        <translation>Invalid block version: %1</translation>
    </message>
    <message>
        <location filename="../block_list.cpp" line="86"/>
        <source>无效的方块id %1</source>
        <translation>Invalid block id: %1</translation>
    </message>
    <message>
        <location filename="../block_list.cpp" line="89"/>
        <source>无效的1.12方块id %1</source>
        <translation>Invalid 1.12 block id: %1</translation>
    </message>
    <message>
        <location filename="../block_list.cpp" line="113"/>
        <source>无效的一组方块数量 %1</source>
        <translation>Invalid stack size: %1</translation>
    </message>
    <message>
        <location filename="../block_list.cpp" line="133"/>
        <source>方块属性needStone必须是bool或者版本数组</source>
        <translation>needStone must be boolean or array of versions</translation>
    </message>
    <message>
        <location filename="../block_list.cpp" line="176"/>
        <source>无法解析 metainfo.json：%1</source>
        <translation>Failed to parse metainfo.json: %1</translation>
    </message>
    <message>
        <location filename="../block_list.cpp" line="186"/>
        <source>无法解压 metainfo.json：%1</source>
        <translation>Failed to extract metainfo.json: %1</translation>
    </message>
    <message>
        <location filename="../block_list.cpp" line="208"/>
        <source>方块列表压缩包中缺少文件 &quot;%1&quot;</source>
        <translation>File &quot;%1&quot; is missing from archive</translation>
    </message>
    <message>
        <location filename="../block_list.cpp" line="220"/>
        <source>无法获取方块列表中文件%1的大小：%2，错误码%3</source>
        <translation>Failed to get size of file %1 in block list: %2; error code %3</translation>
    </message>
    <message>
        <location filename="../block_list.cpp" line="237"/>
        <source>无法从方块列表中解压文件%1：%2</source>
        <translation>Failed to extract %1 from archive: %2</translation>
    </message>
    <message>
        <location filename="../block_list.cpp" line="249"/>
        <source>无法从方块列表中解压文件%1，应有%2字节，实际上只有%3字节：%4</source>
        <translation>Failed to extract %1 from archive, expected %2 bytes but extracted %3 bytes; %4</translation>
    </message>
    <message>
        <location filename="../block_list.cpp" line="279"/>
        <source>方块列表中有metainfo.json，但是解析失败：%1
</source>
        <translation>metainfo.json exist in archive but failed to parse: %1
</translation>
    </message>
    <message>
        <location filename="../block_list.cpp" line="303"/>
        <source>方块列表json应该直接包含json数组</source>
        <translation>JSON should contain an array directly</translation>
    </message>
    <message>
        <location filename="../block_list.cpp" line="321"/>
        <source>无法解析第%1个方块：%2</source>
        <translation>Failed to parse block at index %1: %2</translation>
    </message>
    <message>
        <location filename="../block_list.cpp" line="336"/>
        <source>解析json时遇到异常：%1</source>
        <translation>nlohmann json exception: %1</translation>
    </message>
    <message>
        <location filename="../block_list.cpp" line="350"/>
        <source>%1缺少图片：%2
</source>
        <translation>%1, required by %2
</translation>
    </message>
    <message>
        <location filename="../block_list.cpp" line="365"/>
        <source>无法读取图片%1：%2
</source>
        <translation>Failed to load image %1: %2
</translation>
    </message>
    <message>
        <location filename="../block_list.cpp" line="377"/>
        <source>%1的尺寸错误，应当是16x16，但实际上是%2行%3列
</source>
        <translation>%1 has invalid size. Expected 16x16, but found %2 rows and %3 cols
</translation>
    </message>
    <message>
        <location filename="../block_list.cpp" line="408"/>
        <source>无法打开压缩包%1：%2。libzip错误码：%3</source>
        <translation>Failed to open archive %1: %2. libzip error code: %3</translation>
    </message>
    <message>
        <location filename="../block_list.cpp" line="433"/>
        <source>无法创建zip_source_t：%1</source>
        <translation>Failed to create zip_source_t: %1</translation>
    </message>
    <message>
        <location filename="../block_list.cpp" line="449"/>
        <source>无法打开压缩包，zip_err错误码：%1，sys_err错误码：%2</source>
        <translation>Failed to open archive, zip_err = %1, sys_err = %2</translation>
    </message>
    <message>
        <location filename="../mc_block.cpp" line="114"/>
        <source>内部错误：方块id&quot;%1&quot;通过了检查，但是再次检查出错：%2</source>
        <translation>Internal error: block id &quot;%1&quot; passed checking but faile again: %2</translation>
    </message>
</context>
</TS>
