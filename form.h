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
    void slotDownloadSkipped(const QModelIndex &idx, const QString &reason);
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
    DownloadTableModel *m_model; // model data unduhan
    QQueue<Downloader*> m_downloaderList; // antrian objek untuk mengunduh file
    QQueue<int> m_queue; // antrian unduhan
    QActionGroup *repoGroup; // menangani pemilihan mirror repositori
    QQueue<FileSizeDownloader*> m_fileSizeQueue; // antrian objek untuk mengunduh ukuran file
    QQueue<QModelIndex> m_sizeQueue; // antrian unduhan size file
    QMap<Downloader*, QModelIndex> downloaderMap; // tracking
    SortProxyModel *m_proxyModel; // model perantara untuk melakukan mengurutkan data
    QItemSelectionModel *m_selectionModel; // seleksi item data
    QTimer *m_timer; // timer untuk memeriksa status unduhan
    int m_maks_concurrent; // maksimum koneksi konkuren untuk mengunduh file
    int m_maksFileSizeCount; // maksimum koneksi konkuren untuk mendapat size file
    int m_repoID; // mirror repositori

    void initActions();
    void initRepoComboBoxes();
    void initProxy();
    void initConcurrentConnection();

    void setMaksConcurrent(int val);
    void writeSettings();
    void getFileSize();
    void changeStatusButton(const QModelIndex &idx);
    void verifyDownloads();
};

#endif // FORM_H
