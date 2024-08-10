<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="en_US">
<context>
    <name>BaseColorWidget</name>
    <message>
        <location filename="../BaseColorWidget.ui" line="20"/>
        <source>启用</source>
        <translation>Enable</translation>
    </message>
    <message>
        <location filename="../BaseColor.cpp" line="100"/>
        <source>无法删除方块 %1，基色 %2 只拥有 %3个方块，若继续删除，则该基色将没有方块，SlopeCraft 可能崩溃。</source>
        <translation>Failed to remove %1: base color %2 has only %3 block now, if you continue to delete, there will not be any block for this base color, and SlopeCraft will possible crash.</translation>
    </message>
</context>
<context>
    <name>BlockListManager</name>
    <message>
        <location filename="../BlockListManager.cpp" line="87"/>
        <source>解析方块列表失败</source>
        <translation>Failed to parse block list</translation>
    </message>
    <message>
        <location filename="../BlockListManager.cpp" line="92"/>
        <source>解析方块列表成功，但出现警告</source>
        <translation>Block list parsed with warnings</translation>
    </message>
    <message>
        <location filename="../BlockListManager.cpp" line="119"/>
        <source>无法加载方块列表</source>
        <translation>Failed to parse block list</translation>
    </message>
    <message>
        <location filename="../BlockListManager.cpp" line="120"/>
        <source>名为 %1 的方块列表已经加载，不允许加载同名的方块列表。</source>
        <translation>Block list named %1 was already loaded, and it&apos;s not allowed to load multiple block list with identity name.</translation>
    </message>
    <message>
        <location filename="../BlockListManager.cpp" line="166"/>
        <source>无法删除方块列表 &quot;%1&quot;，没有加载同名的方块列表。已加载：%2</source>
        <translation>Failed to load block list \&quot;%1\&quot;, no block list for such name. Loaded: %2</translation>
    </message>
    <message>
        <location filename="../BlockListManager.cpp" line="213"/>
        <location filename="../BlockListManager.cpp" line="237"/>
        <source>加载预设错误</source>
        <translation>Failed to load preset</translation>
    </message>
    <message>
        <location filename="../BlockListManager.cpp" line="214"/>
        <source>预设文件包含的基色数量 (%1) 与实际情况 (%2) 不符</source>
        <translation>There &apos;re %1 basecolors in the preset file, while actually there are
                %2 basecolors</translation>
    </message>
    <message>
        <location filename="../BlockListManager.cpp" line="238"/>
        <source>预设中为基色%1指定的方块 id 是&quot;%2&quot;，没有找到这个方块 id</source>
        <translation>Block &quot;%2&quot; is assigned to base color %1, but failed to find a
                block with such id</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../BLM_preset.cpp" line="49"/>
        <source>基色 %1 的预设被重复定义。一个基色只能被定义一次。</source>
        <translation>The preset of base color %1 is defined multiple time. A base color can be
                defined only once.</translation>
    </message>
    <message>
        <location filename="../BLM_preset.cpp" line="58"/>
        <source>解析预设 json 时发生异常：&quot;%1&quot;</source>
        <translation>Exception occured when parsing preset json: &quot;%1&quot;</translation>
    </message>
</context>
</TS>
