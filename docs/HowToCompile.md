# How to Complie?

## Libs required:
1. **Qt v6.1.0 or later** for gui
   - later versions may not support windows7
   - If your Qt kit is compiled and configured by yourself, make sure you have `QtBase` and `QtNetwork` installed.
2. **Eigen v3.4.0 or later** for linear algebra
3. [**HeuristicFlow v1.6.2.1**](github.com/ToKiNoBug/HeuristicFlow.git) for mordern optimization algorithms
   - If the repo is not public, draw me an issue to get it.
4. **zlib v1.12.11 or later** for gzip compression

## Projects in SlopeCraft:
|    Project name    | Binary type | Dependents                                    | Description                                              |
| :----------------: | :---------: | :-------------------------------------------- | :------------------------------------------------------- |
|   `imageCutter`    | Executable  | Qt base                                       | A unnecessary image preprocesser                         |
| `BlockListManager` | Static lib  | Qt base, SlopeCraftL's header                 | A class to manage blocks that are avaliable for map arts |
|   `GAConverter`    | Static lib  | Eigen, SlopeCraftL's header, HeuristicFlow    | A converter based on genetic algorithm                   |
|   `SlopeCraftL`    | Shared lib  | Eigen, zlib, GAConverter, HeuristicFlow       | The kernel of SlopeCraft                                 |
|  `SlopeCraftMain`  | Executable  | Qt base, Eigen, SlopeCraftL, BlockListManager | The executable of SlopeCraft                             |

Note that when SlopeCraftL links GAConverter, the link mode is **public**, for GAConverter uses global read-only variables implemented in SlopeCraftL.

## Steps:
1. Install Qt v6.1.0 on your computer. 
   
   It's suggested to install Qt3d together, since they might be called in future version and u won't need to install again if you do that.

   Complier: MingW(gcc). MSVC and clang may not pass the compiling. Your compiler must supports C++ 17 standard.

2. Clone this repo
3. Edit **SlopeCraft/CMakeLists.txt**
   SlopeCraft requires Qt, Eigen, zlib and HeuristicFlow. Among then zlib is usually integrated in the compiler kits, so you don't have to install it manually. But you must change the rest, although they have default values, but they only work on my PC.
   
   Here are cmake variables you need to edit:
   | Variable Name                          | Description                                                                                                                                                              |
   | :------------------------------------- | :----------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
   | `SlopeCraft_Eigen3_include_dir`        | The include directory of Eigen3. Folders named `Eigen` and `unsupported` should be in this folder .                                                                      |
   | `SlopeCraft_HeuristicFlow_include_dir` | The include directory of HeuristicFlow. Folder `HeuristicFlow` and file `READMD.md` should be in this folder.                                                            |
   | `SlopeCraft_Qt_root_dir`               | The root directory of a installed Qt. Example: If the `qmake.exe` is at `D:/Qt/6.1.0/gcc12-shared/bin/qmake.exe`, then the variable should be `D:/Qt/6.1.0/gcc12-shared` |

4. Release translation.

5. Build all projects.

6. Install and delete
   Run cmake --install to install all binaries and block files. The install directory is `${CMAKE_BINARY_DIR}/install`, where you can find all executables. There will also be a directory `please_delete_this_folder`, delete it.

7. Deploy
   You may need to deploy shared libs for executables. For windows, run `windeployqt SlopeCraft.exe` in command line. And there should be similiar deployment commands on other platforms, like `macdeployqt` on MacOS.


## Notice
1. Since Eigen has many compile-time things, and HeuristicFlow may arouse many stupid warnings, your first compiling may spend several minutes and receive tons of warnings. Just wait and ignore them.
2. If you meet any problem, draw me a new issue.
