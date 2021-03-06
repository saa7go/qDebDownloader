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
#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QModelIndex>
#include <QFile>
#include "downloaddata.h"

class QNetworkAccessManager;
class QFile;
class QNetworkReply;

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader(QNetworkAccessManager *manager, QWidget *parent = 0);
    ~Downloader();

signals:
    void progressDownload(const QModelIndex &idx, int progress);
    void progressSize(const QModelIndex &idx, qint64 size);
    void downloadFinish();
    void error(const QModelIndex &idx, const QString &text);
    void downloadSkipped(const QModelIndex &idx, const QString &reason);

public Q_SLOTS:
    void download(const QModelIndex &idx);
    void pause();
    void cancel();

private Q_SLOTS:
    void replyReadRead();
    void replyFinished();
    void replyDeleted();

private:
    QNetworkAccessManager *m_manager;
    QModelIndex m_index;
    QFile m_file;
    QNetworkReply *m_reply;

    int m_progress; // progress indikator (0 - 100)
    DownloadData::Status m_status;

    void downloadProgress(qint64, qint64);
};

#endif // DOWNLOADER_H
