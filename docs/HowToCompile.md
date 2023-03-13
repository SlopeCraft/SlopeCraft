# How to Complie?

## Supported compilers:
   Only gcc is fully supported. MSVC and clang may not pass the compiling. Your compiler must supports C++ 20 standard.

## Libs required:
1. **Qt v6.2.4 or later** for gui
      - later versions may not support windows7
     - If your Qt kit is compiled and configured by yourself, make sure you have `QtBase` and `QtNetwork` installed.
2. **Eigen v3.4.0 or later** for linear algebra
      - Can be downloaded automatically.
3. [**HeuristicFlow v1.6.2.1**](github.com/ToKiNoBug/HeuristicFlow.git) for mordern optimization algorithms
      - If the repo is not public, draw me an issue to get it.
     - Can be downloaded automatically.
4. **zlib v1.12.11 or later** for gzip compression
      - Usually it is installed with compiler so you do not need to compile it.
5. **Nlohmann json**
      - Can be downloaded automatically.
6. **libpng**
      - You must compile and install it, and add to `CMAKE_PREFXI_PATH`.
7. **libzip**
      - You must compile and install it, and add to `CMAKE_PREFXI_PATH`.
8. **fmtlib**
      - Can be download and compiled automatically.
9.  **cli11**
    - Can be downloaded automatically.
10 **magic_enum**
    - Can be downloaded automatically.

## Projects in SlopeCraft:
|    Project name    | Binary type | Dependents(external)                                                             | Description                                              |
| :----------------: | :---------: | :------------------------------------------------------------------------------- | :------------------------------------------------------- |
|   `imageCutter`    | Executable  | Qt base                                                                          | A unnecessary image preprocesser                         |
|    `MapViewer`     | Executable  | Qt base, Eigen, zlib                                                             | A tool to browse Minecraft map data files                |
| `BlockListManager` | Static lib  | Qt base                                                                          | A class to manage blocks that are avaliable for map arts |
|   `GAConverter`    | Static lib  | Eigen, HeuristicFlow                                                             | A converter based on genetic algorithm                   |
|   `SlopeCraftL`    | Shared lib  | Eigen, zlib, HeuristicFlow                                                       | The kernel of SlopeCraft                                 |
|  `SlopeCraftMain`  | Executable  | Qt base, Eigen,                                                                  | The executable of SlopeCraft                             |
|   `VisualCraftL`   | Shared lib  | libpng, libzip, Eigen, zlib, nlohmann json, maigc_enum, fmtlib, OpenCL(optional) | The kernel of VisualCraft                                |
|   `VisualCraft`    | Executable  | Qt base,  nlohmann json, magic_enum                                              | The gui executable of VisualCraft                        |
|       `vccl`       | Executable  | Qt base, magic_enum, fmtlib, cli11                                               | The CLI executable of VisualCraft                        |

Note that when SlopeCraftL links GAConverter, the link mode is **public**, for GAConverter uses global read-only variables implemented in SlopeCraftL.

## Steps:
1. Install Qt on your computer. 
   
2. Clone this repo

3. Configure

   Run cmake to configure this project. The command could be:

   ```bash
   cmake -S . -B ./build -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=gcc 
   ```

   You can use other generators like Ninja, but it may not work on windows, because `windres.exe` can not process spaces in including directories correctly, which will cause compilation errors. This is a bug in GNU bintuils, and Ninja have no way to avoid this error, but mingw makefiles does. On other platforms, windres is no longer used, so the bug is also avoided.

   You may need to pass other parameters to cmake. The cmake script of SlopeCraft recevies following parameters:

   |          Parameter           |  Type  |        Default value        | Description                                                                    |
   | :--------------------------: | :----: | :-------------------------: | :----------------------------------------------------------------------------- |
   |     `CMAKE_PREFIX_PATH`      |  PATH  |             ""              | Tell cmake where to find Qt, zlib, libpng, libzip and GPU api sdk(like OpenCL) |
   |    `CMAKE_INSTALL_PREFIX`    |  PATH  | ${CMAKE_BINARY_DIR}/install | Where to install SlopeCraft.                                                   |
   |     `SlopeCraft_GPU_API`     | STRING |          "OpenCL"           | API used to compute. Valid values : OpenCL, None. Metal may be supported.      |
   | `SlopeCraft_update_ts_files` |  BOOL  |            false            | Whether to update language files before build                                  |

4. Build all targets.

   ```bash
   cmake --build . --parallel
   ```

5. Install and delete
   Run cmake --install to install all binaries and block files. By default the install directory is `${CMAKE_BINARY_DIR}/install`, where you can find all executables. There may also be a directory `please_delete_this_folder`, delete it.

6. Deploy
   You may need to deploy shared libs for executables. For windows, run `windeployqt SlopeCraft.exe` in command line. And there should be similiar deployment commands on other platforms, like `macdeployqt` on MacOS.


## Notice
1. Since Eigen has many compile-time things, and HeuristicFlow may arouse many stupid warnings, your first compiling may spend several minutes and receive tons of warnings. Just wait and ignore them.
2. If you meet any problem, draw me a new issue.
3. If you are building VisualCraftL on linux, some linking problems around libzip may occur. To fix this problem, there are 2 ways possible:
   1. Use a **static archive of libzip** instead of shared lib. But this libzip.a should be compiled with argument **`-fPIC`**, because `libVisualCraftL.so`, a shared lib will link to it. 
      - If your libzip.a links other compress libs(for example, lzma), linking may fail. That is because libzip links to its dependents **privately regardless of whether libzip is built to be a shared lib**. I guess that this is a designing mistake in libzip. So it is **deprecated** to use a static libzip.
   2. Use shared libzip, and copy `libzip.so` and symlinks against it to `CMAKE_CURRENT_BINARY_DIR`.