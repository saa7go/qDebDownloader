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
#ifndef APTWEB_H
#define APTWEB_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkRequest>

class QNetworkAccessManager;

class AptWeb : public QObject
{
    Q_OBJECT
    Q_ENUMS(Distribution)
public:
    enum Distribution {
        Ubuntu_NattyNarWhal_i386 = 0x00,
        Ubuntu_NattyNarWhal_amd64 = 0x01,
        Ubuntu_MaverickMeerkat_i386 = 0x02,
        Ubuntu_MaverickMeerkat_amd64 = 0x03,
        Ubuntu_LucidLynx_i386 = 0x04,
        Ubuntu_LucidLynx_amd64 = 0x05,
        Ubuntu_KarmicKoala_i386 = 0x06,
        Ubuntu_JauntyJackalope_i386 = 0x07,
        Ubuntu_IntrepidIbex_i386 = 0x08,
        Ubuntu_HardyHeron_i386 = 0x09,
        Ubuntu_GutsyGibbon_i386 = 0x0A,
        XUbuntu_GutsyGibbon_i386 = 0x0B,
        Ubuntu_FeistyFawn_i386 = 0x0C,
        Ubuntu_EdgyEft_Server_i386 = 0x0D,
        Ubuntu_DapperDrake_i386 = 0x0E
    };

    explicit AptWeb(QNetworkAccessManager *manager, QObject *parent = 0);
    ~AptWeb();


    void findPackage(const QStringList &packagesName, Distribution dist);
    void findPackage(const QString &packagesName, Distribution dist);
    void setRepoId(int repoId);

    QNetworkAccessManager* manager() const
    {
        return m_manager;
    }

    int repoId() const { return m_repoId; }

Q_SIGNALS:
    void downloadList(const QStringList &packages);
    void error(const QString &text);

private Q_SLOTS:
    void replyFinished();
    void parser(const QString &html);

private:
    QNetworkAccessManager *m_manager;
    int m_repoId;
};

#endif // APTWEB_H
