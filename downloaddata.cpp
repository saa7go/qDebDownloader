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

#include "downloaddata.h"
#include "downloaddataprivate.h"
#include <QDebug>

DownloadData::DownloadData()
{
    d = new DownloadDataPrivate;
    d->selected = true;
    d->size = 0;
    d->fileSize = 0;
}

DownloadData::DownloadData(const QString &url, /*const QString &packageName,*/ int status, int progress)
{
    d = new DownloadDataPrivate;
    int length = url.length();
    int pos = url.lastIndexOf('/') + 1;
    QString packageName = url.right(length - pos);
    d->url = url;
    d->packageName = packageName;
    d->status = status;
    d->progress = progress;
    d->selected = true;
    d->size = 0;
    d->fileSize = 0;
}

DownloadData::DownloadData(const DownloadData &other)
{
    d = other.d;
    d->ref.ref();
}

DownloadData::~DownloadData()
{
//    qDebug() << this << "destroyed";
    if(!d->ref.deref()) {
        delete d;
    }
}

const DownloadData& DownloadData::operator =(const DownloadData &other)
{
    qAtomicAssign(d, other.d);
    return *this;
}

void DownloadData::detach()
{
    qAtomicDetach(d);
}

void DownloadData::setPackageName(const QString &packageName)
{
    detach();
    d->packageName = packageName;
}

void DownloadData::setUrl(const QString &url)
{
    detach();
    d->url = url;
}

void DownloadData::setStatus(int status)
{
    detach();
    d->status = status;
}

void DownloadData::setProgress(int progress)
{
    detach();
    d->progress = progress;
}

void DownloadData::setSelected(bool val)
{
    detach();
    d->selected = val;
}

void DownloadData::setSize(qint64 size)
{
    detach();
    d->size = size;
}

QString DownloadData::packageName() const
{
    return d->packageName;
}

QString DownloadData::url() const
{
    return d->url;
}

int DownloadData::status() const
{
    return d->status;
}

int DownloadData::progress() const
{
    return d->progress;
}

bool DownloadData::selected() const
{
    return d->selected;
}

qint64 DownloadData::size() const
{
    return d->size;
}

void DownloadData::setFileSize(qint64 size)
{
    detach();
    d->fileSize = size;
}

qint64 DownloadData::fileSize() const
{
    return d->fileSize;
}
