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

#ifndef FORM_H
#define FORM_H

#include <QMainWindow>
#include <QQueue>
#include <QAbstractItemView>

class AptWeb;
class QNetworkAccessManager;
class DownloadTableModel;
class Downloader;
class QModelIndex;
class QActionGroup;
class FileSizeDownloader;
class SortProxyModel;

namespace Ui {
    class Form;
}

class Form : public QMainWindow
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = 0);
    void makeCenter();
    ~Form();

private Q_SLOTS:
    void slotSizeReceived(const QModelIndex &idx, qint64 size);
    void setTotalFileSize(const QModelIndex &idx);
    void slotError(const QModelIndex &idx, const QString &error);
    void error(const QString &error);
    void repoChanged(QAction *action);
    void slotDownloadSkipped(const QModelIndex &idx);
    void downloadList(const QStringList &packages);
    void slotDownloadFinished();
    void slotProgressDownload(const QModelIndex &idx, int progress);
    void slotProgressSize(const QModelIndex &idx, qint64 size);
    void slotCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous);
    void timerTimeout();

    void on_pilihSemuaButton_clicked();
    void on_kosongkanSemuaButton_clicked();
    void on_cariButton_clicked();
    void on_unduhButton_clicked();
    void on_actionJumlahUnduhan_triggered();
    void on_actionFolderUnduhan_triggered();
    void on_actionProxy_triggered();
    void on_actionTentang_activated();
    void on_lanjutkanButton_clicked();
    void on_hentikanButton_clicked();
    void on_batalButton_clicked();

private:
    Ui::Form *ui;
    QNetworkAccessManager *m_manager;
    DownloadTableModel *m_model;
    QQueue<Downloader*> m_downloaderList;
    QQueue<int> m_queue;
    QActionGroup *repoGroup;
    QQueue<FileSizeDownloader*> m_fileSizeQueue;
    QQueue<QModelIndex> m_sizeQueue;
    QMap<Downloader*, QModelIndex> downloaderMap; // tracking
    SortProxyModel *m_proxyModel;
    QItemSelectionModel *m_selectionModel;
    int m_maks_concurrent;
    int m_maksFileSizeCount;
    int m_repoID;


    void init();
    void setMaksConcurrent(int val);
    void writeSettings();
    void getFileSize();
    void changeStatusButton(const QModelIndex &idx);
};

#endif // FORM_H
