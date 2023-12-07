/*
 Copyright © 2021-2023  TokiNoBug
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
    along with SlopeCraft. If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/SlopeCraft/SlopeCraft
    bilibili:https://space.bilibili.com/351429231
*/

#ifndef SLOPECRAFT_VISUALCRAFT_VISUALCRAFTL_GLOBAL_H
#define SLOPECRAFT_VISUALCRAFT_VISUALCRAFTL_GLOBAL_H

#ifdef Q_DECL_EXPORT
#undef Q_DECL_EXPORT
#endif
#ifdef Q_DECL_IMPORT
#undef Q_DECL_IMPORT
#endif

#if defined(_MSC_VER) || defined(WIN64) || defined(_WIN64) ||  \
    defined(__WIN64__) || defined(WIN32) || defined(_WIN32) || \
    defined(__WIN32__) || defined(__NT__)
#define Q_DECL_EXPORT __declspec(dllexport)
#define Q_DECL_IMPORT __declspec(dllimport)
#else
#define Q_DECL_EXPORT __attribute__((visibility("default")))
#define Q_DECL_IMPORT __attribute__((visibility("default")))
#endif

#if defined(VISUALCRAFTL_BUILD)
#define VCL_EXPORT Q_DECL_EXPORT
#else
#define VCL_EXPORT Q_DECL_IMPORT
#endif

#ifdef _WIN32
#define VCL_EXPORT_FUN VCL_EXPORT
#else
#define VCL_EXPORT_FUN VCL_EXPORT
#endif

#ifdef VISUALCRAFTL_NOT_INSTALLED
#include <SC_version_buildtime.h>
#else
#include "SC_version_buildtime.h"
#endif

#endif  // SLOPECRAFT_VISUALCRAFT_VISUALCRAFTL_GLOBAL_H