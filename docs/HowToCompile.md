# How to Complie?

## Libs required:
Qt v6.1.0 or later

Eigen v3.4.0 or later

zlib (well, any version that is not too ancient?)

## Steps:
1. Install Qt v6.1.0 on your computer. 
   
   It's suggested to install Qt3d together, since they might be called in future version and u won't need to install again if you do that.

   Complier: MingW.

2. Clone this repo
3. Edit **INCLUDEPATH**
   
   1. Open *Kernel.pro* and *SlopeCraftMain.pro*, edit the **INCLUDEPATH** variable sothat the headers can get access to *Eigen* and *zlib*. 
      - For example, on my PC *Eigen* and *Zlib* are put in a directory named CppLibs, and code below can get access to them:

            INCLUDEPATH += D:\CppLibs\zlibs \
                           D:\CppLibs\eigen-3.4.0

   2. Edit **SOURCES** and **HEADERS** in *SlopeCraftMain.pro* as well. This step is to make sure that zlib can be included. It might be a stupid way but it works.
4. Build project `SlopeCraftL`.
   This project is the kernel of SlopeCraft.
5. Build project `SlopeCraftMain`.
   After **SlopeCraftL3.dll** is generated, copy or move it to the same directory where **SlopeCraft.exe** will be generated. Also don't forget to change variable `LIBS` in *SlopeCraftMain.pro* to tell qmake that the execution will link to library **SlopeCraftL3.dll**.
   
   Remember to copy two directories *Blocks* and *Colors* into the same place where the target is generated. SlopeCraft will try to get access to them once started. If it failes to find any of these files, it will report an error and ask you to find it. If you don't find it, SlopeCraft will terminate itself.


## Notice
1. Since Eigen has tons of compile-time optimization, your first compiling may spend several minutes and send tons of warnings. Just wait and bear it.
2. If you meet any problem when compiling, draw a new issue.
