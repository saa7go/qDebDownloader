/* This file is part of qDebDownloader.
*
* Copyright (c) 2011 - Christian Kurniawan Ginting S. <saa7_go@terralinux.org>
*
* qDebDownloader is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* qDebDownloader is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with qDebDownloader. If not, see <http://www.gnu.org/licenses/>. */
#ifndef DOWNLOADDATA_H
#define DOWNLOADDATA_H

#include <QString>

class DownloadDataPrivate;

class DownloadData
{
public:
    enum Status {
        Nothing = 0,
        Downloading = 1,
        Finished = 2,
        Paused = 3,
        Canceled = 4,
        Waiting = 5
    };

    DownloadData();
    DownloadData(const QString &url, /*const QString &packageName, */int status = 0, int progress = 0);
    DownloadData(const DownloadData &other);
     ~DownloadData();
    const DownloadData& operator = (const DownloadData &other);

    void setUrl(const QString &);
    void setPackageName(const QString &);
    void setStatus(int status);
    void setProgress(int progress);
    void setSelected(bool val);
    void setSize(qint64 size);
    void setFileSize(qint64 size);

    QString url() const;
    QString packageName() const;
    int status() const;
    int progress() const;
    bool selected() const;
    qint64 size() const;
    qint64 fileSize() const;

    void detach();

private:
    DownloadDataPrivate *d;
};

#endif // DOWNLOADDATA_H
