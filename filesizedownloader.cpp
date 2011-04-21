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
#include "filesizedownloader.h"
#include "downloadtablemodel.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDebug>

FileSizeDownloader::FileSizeDownloader(QNetworkAccessManager *manager, QObject *parent) :
    QObject(parent), m_manager(manager)
{
    qDebug() << this << "Created";
}

FileSizeDownloader::~FileSizeDownloader()
{
    qDebug() << this << "Destroyed";
}

void FileSizeDownloader::getSize(const QModelIndex &index)
{
    m_idx = index;
    QNetworkRequest req;
    req.setUrl(QUrl(m_idx.data().toString()));
    QNetworkReply *reply = m_manager->head(req);
    connect(reply, SIGNAL(finished()), SLOT(replyFinished()));
}

void FileSizeDownloader::replyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();
    if(reply->error())
        qDebug() << reply->errorString();

    qint64 size = reply->header(QNetworkRequest::ContentLengthHeader).toULongLong();
    emit sizeReceived(m_idx, size);
}
