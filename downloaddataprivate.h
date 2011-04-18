/* This file is part of apt-offline.
*
* Copyright (c) 2011 - Christian Kurniawan Ginting S. <saa7_go@terralinux.org>
*
* apt-offline is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* apt-offline is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with apt-offline. If not, see <http://www.gnu.org/licenses/>. */

#ifndef DOWNLOADDATAPRIVATE_H
#define DOWNLOADDATAPRIVATE_H

#include <QAtomicInt>

class DownloadDataPrivate
{
public:
    DownloadDataPrivate() : ref(1) {}
    DownloadDataPrivate(const DownloadDataPrivate &other)
        : ref(1),
          url(other.url),
          packageName(other.packageName),
          status(other.status),
          progress(other.progress),
          selected(other.selected),
          size(other.size),
          fileSize(other.fileSize)
    {
    }

    QAtomicInt ref;

    QString url;
    QString packageName;
    int status; // 0 None, 1 Downloading, 2 Finished, 3 Paused, 4 Error, 5 Waiting
    int progress;
    int selected;
    qint64 size;
    qint64 fileSize;
};

#endif // DOWNLOADDATAPRIVATE_H
