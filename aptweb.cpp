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
#include "aptweb.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>
#include <QApplication>
#include <QFile>
#include <QStringList>

AptWeb::AptWeb(QNetworkAccessManager *manager, QObject *parent) :
    QObject(parent), m_manager(manager), m_repoId(0)
{
    qDebug() << this << "Created!";
}

AptWeb::~AptWeb()
{
    qDebug() << this << "Destroyed";
}

void AptWeb::findPackage(const QStringList &packagesName, Distribution dist)
{
    findPackage(packagesName.join(" "), dist);
}

void AptWeb::findPackage(const QString &packagesName, Distribution dist)
{
    QNetworkRequest request;

//    int dist_num = 0;

//    switch(dist)
//    {
//    case Ubuntu_MaverickMeerkat_i386:
//        dist_num = 0; break;
//    case Ubuntu_MaverickMeerkat_amd64:
//        dist_num = 1; break;
//    case Ubuntu_LucidLynx_i386:
//        dist_num = 2; break;
//    case Ubuntu_LucidLynx_amd64:
//        dist_num = 3; break;
//    case Ubuntu_KarmicKoala_i386:
//        dist_num = 4; break;
//    case Ubuntu_JauntyJackalope_i386:
//        dist_num = 5; break;
//    case Ubuntu_IntrepidIbex_i386:
//        dist_num = 6; break;
//    case Ubuntu_HardyHeron_i386:
//        dist_num = 7; break;
//    case Ubuntu_GutsyGibbon_i386:
//        dist_num = 8; break;
//    case XUbuntu_GutsyGibbon_i386:
//        dist_num = 9; break;
//    case Ubuntu_FeistyFawn_i386:
//        dist_num = 10; break;
//    case Ubuntu_EdgyEft_Server_i386:
//        dist_num = 11; break;
//    default: // Ubuntu_DapperDrake_i386
//        dist_num = 12; break;
//    }

    QUrl data;
    data.addQueryItem("dist", QString::number(dist));
    data.addQueryItem("mirror", QString::number(m_repoId));
    data.addQueryItem("packages", packagesName);
    data.addQueryItem("submit", "submit");
    const QByteArray data_encoded = data.toString().remove(0, 1).toAscii();
    qDebug() << data_encoded;
    request.setUrl(QUrl("http://apt-web.dahsy.at"));
    request.setRawHeader("User-Agent", "qDebDownloader 0.1");
    request.setRawHeader("Referer", "http://apt-web.dahsy.at/");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setHeader(QNetworkRequest::ContentLengthHeader, data_encoded.size());
    QNetworkReply *reply = m_manager->post(request, data_encoded);
    connect(reply, SIGNAL(finished()), this, SLOT(replyFinished()));
}


void AptWeb::replyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();

    if(reply->error() != QNetworkReply::NoError)
    {
        QString _error;
        if(reply->error() == QNetworkReply::HostNotFoundError)
            _error = tr("Host tidak ditemukan. Periksa koneksi internet anda.");
        else if(reply->error() == QNetworkReply::TimeoutError)
            _error = tr("Koneksi timeout.");
        else if(reply->error() == QNetworkReply::ProxyNotFoundError)
            _error = tr("Proxy gagal ditemukan. Periksa konfigurasi internet anda");
        else
            _error = reply->errorString();
        emit error(_error);
            return;
    }
    parser(QString(reply->readAll()));
}

void AptWeb::parser(const QString &html)
{
    QString tmp = html;
    QTextStream stream(&tmp);
    QStringList urlList;

    bool found = false;
    while(!found && !stream.atEnd())
    {
        if(stream.readLine() == "<h2>URLs</h2>")
            found = true;
    }

    if(found)
    {
        bool finished = false;
        while(!finished)
        {
            QString text = stream.readLine();
            if(text == "</ul>")
                finished = true;
            else
            {
                QString pattern = "<li><a href=\"";
                if(text.length() < pattern.length())
                    continue;
                int end_pos = text.indexOf('\"', pattern.length());
                urlList.append(text.mid(pattern.length(), end_pos-pattern.length()));
            }
        }
    }

    emit downloadList(urlList);
}

void AptWeb::setRepoId(int repoId)
{
    if(repoId == m_repoId)
        return;
    m_repoId = repoId;
}
