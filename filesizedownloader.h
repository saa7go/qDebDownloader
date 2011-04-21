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
#ifndef FILESIZEDOWNLOADER_H
#define FILESIZEDOWNLOADER_H

#include <QObject>
#include <QQueue>
#include <QModelIndex>

class QNetworkAccessManager;
class DownloadTableModel;

class FileSizeDownloader : public QObject
{
    Q_OBJECT
public:
    explicit FileSizeDownloader(QNetworkAccessManager *manager, QObject *parent = 0);
    ~FileSizeDownloader();
    void getSize(const QModelIndex &index);

Q_SIGNALS:
    // void error(const QModelIndex &idx, const String &error);
    void sizeReceived(const QModelIndex &idx, qint64 size);

private Q_SLOTS:
    void replyFinished();

private:
    QNetworkAccessManager *m_manager;
    DownloadTableModel *m_model;
    QModelIndex m_idx;
};

#endif // FILESIZEDOWNLOADER_H
