/*
 Copyright © 2021  TokiNoBug
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
    along with SlopeCraft.  If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/ToKiNoBug
    bilibili:https://space.bilibili.com/351429231
*/

#ifndef BLOCKLISTMANAGER_GLOBAL_H
#define BLOCKLISTMANAGER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(BLOCKLISTMANAGER_LIBRARY)
#  define BLOCKLISTMANAGER_EXPORT Q_DECL_EXPORT
#else
#  define BLOCKLISTMANAGER_EXPORT Q_DECL_IMPORT
#endif

#endif // BLOCKLISTMANAGER_GLOBAL_H
