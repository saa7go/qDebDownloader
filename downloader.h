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

#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QModelIndex>
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
//    void downloadSkipped(const QModelIndex &idx);

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
    QFile *m_file;
    QNetworkReply *m_reply;

    int m_progress; // progress indikator (0 - 100)
//    bool m_isPaused; // indikator pause
//    bool m_isDownloading; // indikator mengunduh
//    bool m_isFinished; // indikator unduhan selesai
//    bool m_isResuming; // indikator unduhan dari proses resume
//    bool m_isCanceled; // indikator unduhan dibatalkan
    DownloadData::Status m_status;

    void downloadProgress(qint64, qint64);
};

#endif // DOWNLOADER_H
