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

#include "downloader.h"
#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <QMessageBox>
#include <QSettings>
#include <QDir>
#include <QAbstractItemModel>

Downloader::Downloader(QNetworkAccessManager *manager, QWidget *parent) :
    QObject(parent), m_manager(manager),
    m_progress(0), m_status(DownloadData::Nothing)
{
    qDebug() << this << "Created";
}

Downloader::~Downloader()
{
    qDebug() << this << "Destroyed";
}

void Downloader::download(const QModelIndex &idx)
{
    if(!idx.isValid())
        return;
    m_progress = 0;
    m_index = idx;
    m_status = DownloadData::Nothing;

    QString fileName = idx.model()->index(idx.row(), 1).data().toString();
    QSettings settings("./apt-web.ini", QSettings::IniFormat);
    QString dirPath = settings.value("lokasi-folder-unduhan", QString()).toString();
    if(!dirPath.isEmpty())
    {
        QDir dir(dirPath);
        if(dir.exists())
            fileName =  QDir::toNativeSeparators(dir.path()) + QDir::separator() + fileName;
    }

    QUrl url(m_index.data().toString());
    if(!url.isValid())
        return;

    m_file.setFileName(fileName);
    QIODevice::OpenMode openMode = QIODevice::WriteOnly;
    int status = m_index.model()->index(m_index.row(), 2).data(Qt::EditRole).toInt();
    bool continueDownload = false;

    if(status == DownloadData::Paused || status == DownloadData::Waiting) // paused atau waiting
    {
        continueDownload = true;
        openMode |= QIODevice::Append;
    }

    if(!m_file.open(openMode))
    {
        emit error(m_index, tr("TIDAK BISA MENULIS FILE"));
        return;
    }

    QNetworkRequest req;
    req.setUrl(url);
    if(continueDownload)
    {
        qint64 start = m_file.size();
        qint64 end = m_index.model()->index(m_index.row(), 5).data(Qt::EditRole).toLongLong()-1;
        QString str = QString("bytes=%1-%2").arg(QString::number(start)).arg(QString::number(end));
        req.setRawHeader("Range",  str.toAscii());
    }

    m_reply = m_manager->get(req);
    connect(m_reply, SIGNAL(readyRead()), SLOT(replyReadRead()));
    connect(m_reply, SIGNAL(finished()), SLOT(replyFinished()));
    connect(m_reply, SIGNAL(destroyed()), this, SLOT(replyDeleted()));

    QAbstractItemModel *model = const_cast<QAbstractItemModel*>(m_index.model());
    model->setData(model->index(m_index.row(), 2), DownloadData::Downloading);
    m_status = DownloadData::Downloading;
}

void Downloader::replyReadRead()
{
    if(m_reply->error() != QNetworkReply::NoError)
        return;

    QByteArray arr = m_reply->readAll();
    m_file.write(arr);
    downloadProgress(m_file.size(), m_index.model()->index(m_index.row(), 5).data(Qt::EditRole).toLongLong());
}

void Downloader::replyFinished()
{
    if(m_reply->error() != QNetworkReply::NoError)
    {
        if(m_reply->error() != QNetworkReply::OperationCanceledError) // disengaja
        {
            m_file.remove();
            qDebug() << m_reply->errorString();
            emit error(m_index, m_reply->errorString());
        }
    }

    m_reply->deleteLater();
    m_reply = 0;

    QAbstractItemModel *model = const_cast<QAbstractItemModel*>(m_index.model());

    if(m_file.size() == model->index(m_index.row(), 5).data(Qt::EditRole).toLongLong())
        m_status = DownloadData::Finished; //        m_isFinished = true;

    m_file.close();

    if(m_status == DownloadData::Paused)
    {
        model = const_cast<QAbstractItemModel*>(m_index.model());
        model->setData(model->index(m_index.row(), 2), DownloadData::Paused);
    }
    else if(m_status == DownloadData::Canceled)
    {
        model = const_cast<QAbstractItemModel*>(m_index.model());
        model->setData(model->index(m_index.row(), 2), 4);
        model->setData(model->index(m_index.row(), 3), 100);
        m_file.remove();
    }
    else if(m_status == DownloadData::Finished)
    {
        model = const_cast<QAbstractItemModel*>(m_index.model());
        model->setData(model->index(m_index.row(), 2), DownloadData::Finished);
    }

    m_status = DownloadData::Nothing;
    emit downloadFinish();
}

void Downloader::pause()
{
    int status = m_index.model()->index(m_index.row(), 2).data(Qt::EditRole).toInt();
    if(status == DownloadData::Downloading)
    {
        m_status = DownloadData::Paused;
        m_reply->abort();
    }
}

void Downloader::cancel()
{
    int status = m_index.model()->index(m_index.row(), 2).data(Qt::EditRole).toInt();
    if(status == DownloadData::Downloading || status == DownloadData::Paused || status == DownloadData::Waiting)
    {
        m_status = DownloadData::Canceled;
        m_reply->abort();
    }
}

void Downloader::downloadProgress(qint64 bytesReceived, qint64 total)
{
    int prog = double(bytesReceived) / total * 100;
    if(m_progress != prog)
    {
        m_progress = prog;
        emit progressDownload(m_index, m_progress);
    }
    emit progressSize(m_index, bytesReceived);
}

void Downloader::replyDeleted()
{
    qDebug() << "Reply is deleted!";
}
