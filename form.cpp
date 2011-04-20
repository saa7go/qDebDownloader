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

#include "form.h"
#include "ui_form.h"
#include <QTimer>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QActionGroup>
#include <QFile>
#include <QInputDialog>
#include <QFileDialog>
#include <QSettings>
#include <QNetworkAccessManager>
#include <QDebug>

#include "aptweb.h"
#include "downloadtablemodel.h"
#include "downloader.h"
#include "progressbardelegate.h"
#include "proxydialog.h"
#include "aboutdialog.h"
#include "filesizedownloader.h"
#include "helper.h"
#include "sortproxymodel.h"


#define FILESIZE_GETTER_COUNT 6

Form::Form(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Form),
    m_manager(new QNetworkAccessManager(this)),
    m_model(new DownloadTableModel(this)),
    m_proxyModel(new SortProxyModel(this)),
    m_timer(new QTimer(this)),
    m_maksFileSizeCount(FILESIZE_GETTER_COUNT)
{
    ui->setupUi(this);

    initRepoComboBoxes();
    initActions();
    initProxy();
    initConcurrentConnection();

    // inisialiasi model pada treeview
    m_proxyModel->setSourceModel(m_model);
    ui->treeView->setModel(m_proxyModel);
    ui->treeView->header()->setResizeMode(0, QHeaderView::ResizeToContents);
    ui->treeView->setItemDelegateForColumn(4, new ProgressBarDelegate(this)); // progress
    ui->treeView->header()->hideSection(1); // url
    ui->treeView->header()->hideSection(3); // status
    ui->treeView->header()->setMovable(false);
    m_selectionModel = ui->treeView->selectionModel();
    ui->cariPaketLineEdit->setFocus();
    ui->buttonWidget->hide();
    m_timer->setInterval(500);

    connect(ui->actionTentang_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), SLOT(setTotalFileSize(QModelIndex)));
    connect(m_timer, SIGNAL(timeout()), SLOT(timerTimeout()));

    // ganti font
#ifdef Q_WS_WIN
    // note: di windows xp default font terlihat sangat kecil
    // jadi saya coba set ke 10
    QFont tmpFont = qApp->font();
    tmpFont.setPointSize(10);
    qApp->setFont(tmpFont);
#endif
}

Form::~Form()
{
    writeSettings();
    delete ui;
    qDebug() << this << "destroyed!";
}

void Form::initRepoComboBoxes()
{
    QComboBox *cb = ui->distribusiComboBox;
    cb->addItem("Ubuntu 10.10 \"Maverick Meerkat\" i386", 0);
    cb->addItem("Ubuntu 10.10 \"Maverick Meerkat\" amd64", 1);
    cb->addItem("Ubuntu 10.04 \"Lucid Lynx\" i386", 2);
    cb->addItem("Ubuntu 10.04 \"Lucid Lynx\" amd64", 3);
    cb->addItem("Ubuntu 9.10 \"Karmic Koala\" i386", 4);
    cb->addItem("Ubuntu 9.04 \"Jaunty Jackalope\" i386", 5);
    cb->addItem("Ubuntu 8.10 \"Intrepid Ibex\" i386", 6);
    cb->addItem( "Ubuntu 8.04 \"Hardy Heron\" i386", 7);
    cb->addItem("Ubuntu 7.10 \"Gutsy Gibbon\" i386", 8);
    cb->addItem("XUbuntu 7.10 \"Gutsy Gibbon\" i386", 9);
    cb->addItem("Ubuntu 7.04 \"Feisty Fawn\" i386", 10);
    cb->addItem("Ubuntu 6.10 \"Edgy Eftt\" Server i386", 11);
    cb->addItem("Ubuntu 6.06 \"Dapper Drake\" i386", 12);
}

void Form::initActions()
{
    QAction *action = new QAction(ui->cariPaketLineEdit);
    action->setAutoRepeat(false);
    action->setShortcut(QKeySequence(Qt::Key_Return));
    connect(action, SIGNAL(triggered()), this, SLOT(on_cariButton_clicked()));
    ui->cariPaketLineEdit->addAction(action);

    repoGroup = new QActionGroup(this);
    QAction *kambingUIAction = repoGroup->addAction("Kambing UI");
    kambingUIAction->setData(0);
    QAction *fossIDAction = repoGroup->addAction("FOSS-id");
    fossIDAction->setData(1);
    QAction *komoVlsmAction = repoGroup->addAction("Komo vlsm.org");
    komoVlsmAction->setData(2);
    QAction *indikaNetAction = repoGroup->addAction("Indika.Net");
    indikaNetAction->setData(3);
    QAction *unejAction = repoGroup->addAction("Unej");
    unejAction->setData(4);
    QAction *ugmAction = repoGroup->addAction("UGM");
    ugmAction->setData(5);
    QAction *itsAction = repoGroup->addAction("ITS");
    itsAction->setData(6);
    QAction *buayaKlasAction = repoGroup->addAction("Buaya KLAS");
    buayaKlasAction->setData(7);
    QAction *keboVlsmAction = repoGroup->addAction("Kebo vlsm.org");
    keboVlsmAction->setData(8);
    QAction *itbAction = repoGroup->addAction("ITB");
    itbAction->setData(9);
    QAction *shloVlsmAction = repoGroup->addAction("SHLO vlsm.org");
    shloVlsmAction->setData(10);

    for(int i = 0; i < repoGroup->actions().count(); i++)
        repoGroup->actions()[i]->setCheckable(true);

    QMenu  *menuRepositori = new QMenu("Repositori", this);
    ui->menu_Pengaturan->insertMenu(ui->actionFolderUnduhan, menuRepositori);
    menuRepositori->addActions(repoGroup->actions());
    QSettings settings("./apt-web.ini", QSettings::IniFormat);
    m_repoID = settings.value("mirror-repo", 0).toInt();

    if(m_repoID < 0)
        m_repoID = 1;
    else if(m_repoID > (repoGroup->actions().count() - 1))
        m_repoID = repoGroup->actions().count() - 1;

    for(int i = 0; i < repoGroup->actions().count(); i++)
    {
        if(repoGroup->actions().at(i)->data().toInt() == m_repoID)
        {
            repoGroup->actions()[i]->setChecked(true);
            break;
        }
    }
    connect(repoGroup, SIGNAL(triggered(QAction*)), SLOT(repoChanged(QAction*)));
}

void Form::initProxy()
{
    QSettings settings("./apt-web.ini", QSettings::IniFormat);
    int idxProxy = settings.value("Proxy/Type", 0).toInt();
    if(idxProxy < 0 || idxProxy > 3)
        idxProxy = 0;

    QNetworkProxy proxy;
    if(idxProxy == 0)
        proxy.setType(QNetworkProxy::NoProxy);
    else if(idxProxy == 1)
        proxy.setType(QNetworkProxy::DefaultProxy);
    else {
        if(idxProxy ==  2)
            proxy.setType(QNetworkProxy::HttpProxy);
        else if(idxProxy == 3)
            proxy.setType(QNetworkProxy::Socks5Proxy);
        else
            return;

        proxy.setHostName(settings.value("Proxy/Host").toString());
        proxy.setPort(settings.value("Proxy/Port", 0).toInt());
        proxy.setUser(settings.value("Proxy/Username").toString());
        proxy.setPassword(settings.value("Proxy/Password").toString());
    }
    m_manager->setProxy(proxy);
}

void Form::initConcurrentConnection()
{
    QSettings settings("./apt-web.ini", QSettings::IniFormat);
    int maks_concurrent = settings.value("maks-concurrent", 1).toInt();

    if(maks_concurrent < 1)
        maks_concurrent = 1;
    else if (maks_concurrent > 6)
        maks_concurrent = 6;
    m_maks_concurrent = maks_concurrent;
}


void Form::on_cariButton_clicked()
{
    if(ui->cariPaketLineEdit->text().isEmpty())
        return;

    m_model->setUrlList(QStringList());
    ui->terunduhLabel->setText("0 B");

    ui->cariButton->setEnabled(false);
    ui->unduhButton->setEnabled(false);
    ui->cariPaketLineEdit->setEnabled(false);
    ui->statusbar->showMessage(tr("Mencari paket-paket yg dibutuhkan..."));
    ui->pilihSemuaButton->setEnabled(false);
    ui->kosongkanSemuaButton->setEnabled(false);

    AptWeb::Distribution dist;
    switch(ui->distribusiComboBox->itemData(ui->distribusiComboBox->currentIndex()).toInt())
    {
    case 0:
        dist = AptWeb::Ubuntu_MaverickMeerkat_i386; break;
    case 1:
        dist = AptWeb::Ubuntu_MaverickMeerkat_amd64; break;
    case 2:
        dist = AptWeb::Ubuntu_LucidLynx_i386; break;
    case 3:
        dist = AptWeb::Ubuntu_LucidLynx_amd64; break;
    case 4:
        dist = AptWeb::Ubuntu_KarmicKoala_i386; break;
    case 5:
        dist = AptWeb::Ubuntu_JauntyJackalope_i386; break;
    case 6:
        dist = AptWeb::Ubuntu_IntrepidIbex_i386; break;
    case 7:
        dist = AptWeb::Ubuntu_HardyHeron_i386; break;
    case 8:
        dist = AptWeb::Ubuntu_GutsyGibbon_i386; break;
    case 9:
        dist = AptWeb::XUbuntu_GutsyGibbon_i386; break;
    case 10:
        dist = AptWeb::Ubuntu_FeistyFawn_i386; break;
    case 11:
        dist = AptWeb::Ubuntu_EdgyEft_Server_i386; break;
    case 12:
        dist = AptWeb::Ubuntu_DapperDrake_i386; break;
    }

    AptWeb *aptWeb = new AptWeb(m_manager, this);
    aptWeb->setRepoId(m_repoID);
    connect(aptWeb, SIGNAL(downloadList(QStringList)), this, SLOT(downloadList(QStringList)));
    connect(aptWeb, SIGNAL(error(QString)), this, SLOT(error(QString)));
    aptWeb->findPackage(ui->cariPaketLineEdit->text().trimmed(), dist);
    ui->totalLabel->setText("0 B");
}

void Form::downloadList(const QStringList &packages)
{
    sender()->setParent(0);
    sender()->deleteLater();
    m_model->setUrlList(packages);

    if(packages.count() > 0)
    {
        getFileSize();
        ui->statusbar->showMessage("Mendapatkan ukuran file yang akan diunduh...");
    }
    else
    {
        ui->unduhButton->setEnabled(false);
        ui->pilihSemuaButton->setEnabled(false);
        ui->kosongkanSemuaButton->setEnabled(false);
        ui->cariButton->setEnabled(true);
        ui->cariPaketLineEdit->setEnabled(true);
        ui->statusbar->showMessage(tr("Paket telah terinstall atau anda salah memasukkan nama paket yang dicari"), 5000);
    }
}

void Form::on_unduhButton_clicked()
{
    if(m_proxyModel->rowCount() == 0)
        return;

    for(int i = 0; i < m_proxyModel->rowCount(); i++)
    {
        QModelIndex idx = m_proxyModel->mapToSource(m_proxyModel->index(i, 0)); // checked or not
        int val = idx.data(Qt::CheckStateRole).toInt();
        if(val > 0)
            m_queue.enqueue(idx.row());
        m_model->setData(m_model->index(idx.row(), 4), 0); // progress
        m_model->setData(m_model->index(idx.row(), 5), QString(), Qt::DisplayRole); // pastikan kalau ada info error, info tersebut dihapus
        m_model->setData(m_model->index(idx.row(), 5), 0); // current filesize
    }

    // jika antrian unduhan tidak ada keluar dari fungsi ini
    if(m_queue.count() == 0)
    {
        QMessageBox::warning(this, tr("Unduhan"), tr("Harap pilih minimal satu paket yang ingin anda untuh."));
        return;
    }

    statusBar()->clearMessage();
    m_model->setCheckEnabled(false);
    ui->cariButton->setEnabled(false);
    ui->unduhButton->setEnabled(false);
    ui->pilihSemuaButton->setEnabled(false);
    ui->kosongkanSemuaButton->setEnabled(false);
    ui->cariPaketLineEdit->setEnabled(false);
    ui->buttonWidget->show();

    qApp->processEvents();

    downloaderMap.clear(); // kosongkan pemetaan objek kelas Downloader dengan QModelIndex sebelumnya

    for(int i = 0; i < m_maks_concurrent; i++)
    {
        Downloader *downloader = new Downloader(m_manager, this);
        downloader->setObjectName(QString("Downloader-%1").arg(i));
        m_downloaderList.enqueue(downloader);
        connect(downloader, SIGNAL(error(QModelIndex,QString)), SLOT(slotError(QModelIndex,QString)));
        connect(downloader, SIGNAL(progressSize(QModelIndex,qint64)), SLOT(slotProgressSize(QModelIndex,qint64)));
        connect(downloader, SIGNAL(progressDownload(QModelIndex,int)), SLOT(slotProgressDownload(QModelIndex,int)));
        connect(downloader, SIGNAL(downloadFinish()), SLOT(slotDownloadFinished()));
    }

    m_timer->start();
    while(m_queue.count() > 0)
    {
        if(m_downloaderList.count() > 0)
        {
            int curr = m_queue.dequeue();
            Downloader *dl = m_downloaderList.dequeue();
            QModelIndex idx = m_model->index(curr, 1); // url
            m_model->setData(m_model->index(curr, 3), DownloadData::Downloading); // status
            dl->download(idx);
            downloaderMap[dl] = idx;
        }
        else {
            // jika Downloader telah terpakai semua
            // keluar dari loop
            break;
        }
    }
    ui->treeView->header()->hideSection(0);
    m_proxyModel->setHideUncheked(true);
}

void Form::slotDownloadFinished()
{
    Downloader *downloader = qobject_cast<Downloader*>(sender());
    if(downloader) // periksa jika fungsi ini diterima dari objek Downloader
    {
        QModelIndex idx = downloaderMap.value(downloader);
        if(!idx.isValid()) // tidak mengunduh
        {
            // seharusnya tidak sampai pada baris ini
            qDebug() << "slotDownloadFinished: sedang tidak mengunduh!";
        }
        else {
//            downloaderMap.remove(downloader);
            downloaderMap[downloader] = QModelIndex();
            m_downloaderList.enqueue(downloader);
        }
    }

    qDebug() << "jumlah downloder tersisa:" << m_downloaderList.count();

    verifyDownloads();
}

void Form::slotProgressDownload(const QModelIndex &idx, int progress)
{
    if(!idx.isValid())
        return;
    QModelIndex progIdx =  m_model->index(idx.row(), 4); // progress
    bool retVal = m_model->setData(progIdx, progress);
    if(!retVal)
        qDebug() << "Error";
}

void Form::slotProgressSize(const QModelIndex &idx, qint64 size)
{
    if(!idx.isValid())
        return;
    QModelIndex sizeIdx = m_model->index(idx.row(), 5); // current file size
    bool retVal = m_model->setData(sizeIdx, size, Qt::EditRole);

    if(!retVal)
        qDebug() << "Error";
}

void Form::on_pilihSemuaButton_clicked()
{
    for(int i = 0; i < m_model->rowCount(); i++)
        m_model->setData(m_model->index(i, 0), Qt::Checked, Qt::CheckStateRole); // checked?
}

void Form::on_kosongkanSemuaButton_clicked()
{
    for(int i = 0; i < m_model->rowCount(); i++)
        m_model->setData(m_model->index(i, 0), Qt::Unchecked, Qt::CheckStateRole); // checked ?
}

void Form::slotError(const QModelIndex &idx, const QString &error)
{
    // kembalikan objek Downloader ke dalam antrian
    Downloader *dl = qobject_cast<Downloader*>(sender());
    if(dl) {
        downloaderMap[dl] = QModelIndex();
        m_downloaderList.enqueue(dl);
    }
    m_model->setData(m_model->index(idx.row(), 3), DownloadData::Canceled); // status
    m_model->setData(m_model->index(idx.row(), 4), 100); // progress
    m_model->setData(m_model->index(idx.row(), 5), error, Qt::DisplayRole); // current file size info
    verifyDownloads();
}

void Form::slotDownloadSkipped(const QModelIndex &idx)
{
    QModelIndex sizeIdx = m_model->index(idx.row(), 5); // file size
    m_model->setData(sizeIdx, "DIABAIKAN");
}

void Form::repoChanged(QAction *action)
{
    m_repoID = action->data().toInt();
}

void Form::setMaksConcurrent(int val)
{
    m_maks_concurrent = val;
}

void Form::writeSettings()
{
    QSettings settings("./apt-web.ini", QSettings::IniFormat);
    settings.setValue("mirror-repo", m_repoID);
}

void Form::on_actionJumlahUnduhan_triggered()
{
    bool ok;
    int val = QInputDialog::getInt(this, "Jumlah unduhan", "Nilai(1-6)", 
m_maks_concurrent, 1, 6, 1, &ok);
    if(ok)
    {
        setMaksConcurrent(val);
        QSettings settings("./apt-web.ini", QSettings::IniFormat);
        settings.setValue("maks-concurrent", val);
    }
}

void Form::on_actionFolderUnduhan_triggered()
{
    QSettings settings("./apt-web.ini", QSettings::IniFormat);

    QString dir = QFileDialog::getExistingDirectory(this, "Direktori Unduhan", settings.value("lokasi-folder-unduhan", QString()).toString());
    if(dir.isEmpty())
        return;

    settings.setValue("lokasi-folder-unduhan", dir);
}

void Form::on_actionProxy_triggered()
{
    ProxyDialog dialog(this);
    int val = dialog.exec();
    if(val == QDialog::Accepted)
        m_manager->setProxy(dialog.proxy());
}

void Form::error(const QString &error)
{
    sender()->deleteLater();
    QMessageBox::critical(this, "Error", error);
    ui->cariButton->setEnabled(true);
    ui->cariPaketLineEdit->setEnabled(true);
    ui->statusbar->clearMessage();
}

void Form::on_actionTentang_activated()
{
    AboutDialog dlg(this);
    dlg.exec();
}

void Form::getFileSize()
{
    for(int i = 0; i < m_model->rowCount(); i++)
        m_sizeQueue.enqueue(m_model->index(i, 1)); // url
    m_maksFileSizeCount = (m_sizeQueue.count() < FILESIZE_GETTER_COUNT ? m_sizeQueue.count() : FILESIZE_GETTER_COUNT);
    for(int i = 0; i < m_maksFileSizeCount; i++)
    {
        FileSizeDownloader *fileSizeDownloader = new FileSizeDownloader(m_manager, this);
        fileSizeDownloader->setObjectName(QString("FileSizeDownloader-%1").arg(i));
        connect(fileSizeDownloader, SIGNAL(sizeReceived(QModelIndex,qint64)), SLOT(slotSizeReceived(QModelIndex,qint64)));
        m_fileSizeQueue.enqueue(fileSizeDownloader);
    }

    while(m_sizeQueue.count() > 0)
    {
        if(m_fileSizeQueue.count() > 0)
            m_fileSizeQueue.dequeue()->getSize(m_sizeQueue.dequeue());
        else
            break;
    }
}

void Form::slotSizeReceived(const QModelIndex &idx, qint64 size)
{
    FileSizeDownloader *fileSizeDownloader = qobject_cast<FileSizeDownloader*>(sender());
    m_fileSizeQueue.enqueue(fileSizeDownloader);

    QModelIndex fileSizeIndex = m_model->index(idx.row(), 6); // real file size
    m_model->setData(fileSizeIndex, size);

    if(m_sizeQueue.count() > 0)
        m_fileSizeQueue.dequeue()->getSize(m_sizeQueue.dequeue());

    if(m_fileSizeQueue.count() == m_maksFileSizeCount
            && m_sizeQueue.count() == 0) // sudah selesai
    {
        while(m_fileSizeQueue.count() > 0)
        {
            QObject *obj = m_fileSizeQueue.dequeue();
            obj->setParent(0);
            obj->deleteLater();
        }
        ui->cariPaketLineEdit->setEnabled(true);
        ui->cariButton->setEnabled(true);
        ui->unduhButton->setEnabled(true);
        ui->pilihSemuaButton->setEnabled(true);
        ui->kosongkanSemuaButton->setEnabled(true);
        ui->statusbar->showMessage(QString("Daftar paket yang dibutuhkan berjumlah %1 dengan total ukuran %2").arg(m_model->rowCount()).arg(ui->totalLabel->text()), 5000);
    }
}

// perlu dibenahi
void Form::setTotalFileSize(const QModelIndex &idx)
{
    if (idx.column() == 6 || idx.column() == 5 || idx.column() == 0)
    {
        int column;
        qint64 total = 0;
        double dTotal;
        QString ukuran;

        if(idx.column() == 6 || idx.column() == 0)
            column = 6;
        else
            column = 5;

        for(int i = 0; i < m_model->rowCount(); i++)
        {
            if(m_model->index(i, 0).data(Qt::CheckStateRole).toInt() > 0)
                total += m_model->index(i, column).data(Qt::EditRole).toLongLong();
        }

        AptOffline::caculateSize(total, &dTotal, &ukuran);

        if(column == 5)
            ui->terunduhLabel->setText(QString("%1 %2").arg(QString::number(dTotal, 'f', 2)).arg(ukuran));

        if(column == 6)
            ui->totalLabel->setText(QString("%1 %2").arg(QString::number(dTotal, 'f', 2)).arg(ukuran));
    }
}

void Form::on_lanjutkanButton_clicked()
{
//    if(!ui->treeView->currentIndex().isValid())
//        return;
    if(!m_selectionModel->hasSelection())
        return;
    if(m_selectionModel->selectedIndexes().count() == 0)
        return;

    qDebug() << "Lanjutkan";
    ui->lanjutkanButton->setEnabled(false);
    ui->hentikanButton->setEnabled(true);
    ui->batalButton->setEnabled(true);

//    QModelIndex idx = m_proxyModel->mapToSource(ui->treeView->currentIndex());
    QModelIndex idx = m_proxyModel->mapToSource(m_selectionModel->selectedIndexes().at(0));
    if(!idx.isValid())
        return;

    idx = m_model->index(idx.row(), 3); // status
    if(idx.data(Qt::EditRole).toInt() != DownloadData::Paused) // cek jika status unduha dalam keadaan PAUSE atau tidak
        return;

    idx = m_model->index(idx.row(), 1); // url
    if(!idx.isValid())
    {
        qDebug() << "Tidak valid!";
        return;
    }

    // cek jika ada Downloader yg nganggur
    if(m_downloaderList.count() > 0)
    {
        // jika ada, melanjutkan unduhan
        Downloader *dl = m_downloaderList.dequeue();
        dl->download(idx);
        downloaderMap[dl] = idx;
    }
    else // jika Downloader terpakai semua
    {
        // ganti status menjadi WAITING
        m_model->setData(m_model->index(idx.row(), 3), DownloadData::Waiting); // status
        // masukkan kembali ke antrian unduhan
        m_queue.prepend(idx.row());
    }
}

void Form::on_hentikanButton_clicked()
{
//    if(!ui->treeView->currentIndex().isValid())
//        return;
    if(!m_selectionModel->hasSelection())
        return;
    if(m_selectionModel->selectedIndexes().count() == 0)
        return;

    ui->hentikanButton->setEnabled(false);
    ui->lanjutkanButton->setEnabled(true);
    ui->batalButton->setEnabled(true);

    QModelIndex idx = m_proxyModel->mapToSource(m_selectionModel->selectedIndexes().at(0));
    idx = idx.model()->index(idx.row(), 3); // status

    // cek apakah benar sedang mengunduh
    if(idx.data(Qt::EditRole).toInt() != DownloadData::Downloading)
        return;

    idx = m_model->index(idx.row(), 1); // url
    if(!idx.isValid())
    {
        qDebug() << "Ada error";
        return;
    }

    Downloader *dl = downloaderMap.key(idx);
    if(dl == 0)
    {
        qDebug() << "Downloader tidak ditemukan saat melakukan pause!";
        return;
    }
    qDebug() << "Pause unduhan" << idx.data().toString();
    dl->pause();
}

void Form::on_batalButton_clicked()
{
//    if(ui->treeView->currentIndex().isValid())
    if(m_selectionModel->hasSelection() && m_selectionModel->selectedIndexes().count() > 0)
    {
//        QModelIndex idx = m_proxyModel->mapToSource(ui->treeView->currentIndex());
        QModelIndex idx = m_proxyModel->mapToSource(m_selectionModel->selectedIndexes().at(0));
        idx = m_model->index(idx.row(), 3); // status
        int status = idx.data(Qt::EditRole).toInt();
        idx = m_model->index(idx.row(), 1); // url

        // jika unduhan dalam keadaan sedang mengunduh
        if(status == DownloadData::Downloading)
        {
            Downloader *dl = downloaderMap.key(idx);

            // pastikan Downloader valid
            if(dl != 0)
            {
                qDebug() << "Batal";
                dl->cancel(); // batalkan unduhan, pada kondisi ini penghapusan file dikelola oleh objek Downloader
            }
            else
            {
                qDebug() << "Downloader tidak ditemukan!";
            }

        }
        // jika unduhan dalam keadaan PAUSED atau menunggu untuk diunduh
        else if(status == DownloadData::Waiting || status == DownloadData::Paused)
        {
            QSettings settings("./apt-web.ini", QSettings::IniFormat);
            QDir downloadDir(settings.value("lokasi-folder-unduhan", QString()).toString());
            QString filePath = m_model->index(idx.row(), 2).data().toString(); // default di current dir

            if(downloadDir.exists())
                filePath = QDir::toNativeSeparators(downloadDir.path()) + QDir::separator() + filePath;

            QFile file(filePath);
            if(file.exists())
            {
                // jika status unduhan dalam keadaan menunggu
                // maka antrian yg sebelumnya ditambahkan harus dihapus
                if(status == DownloadData::Waiting)
                    m_queue.removeAll(idx.row());

                file.remove(); // hapus unduhan yg dibatalkan
                // ganti status unduhan menjadi CANCELED
                m_model->setData(m_model->index(idx.row(), 3), DownloadData::Canceled); // status
                m_model->setData(m_model->index(idx.row(), 4), 100); // progress
            }

            // cek jika sudah tidak ada unduhan
//            if(m_queue.count() == 0)
            verifyDownloads();
        }
    }
}

void Form::slotCurrentRowChanged(const QModelIndex &current, const QModelIndex &)
{
    if(current.isValid())
        changeStatusButton(m_proxyModel->mapToSource(current));
}

void Form::changeStatusButton(const QModelIndex &idx)
{
    if(idx.isValid())
    {
        int status = m_model->index(idx.row(), 3).data(Qt::EditRole).toInt(); // status

        if(status == 0 || status == 2 || status == 4)
        {
            ui->lanjutkanButton->setEnabled(false);
            ui->hentikanButton->setEnabled(false);
            ui->batalButton->setEnabled(false);
        }
        else if(status == 1)
        {
            ui->lanjutkanButton->setEnabled(false);
            ui->hentikanButton->setEnabled(true);
            ui->batalButton->setEnabled(true);
        }
        else if(status == 3)
        {
            ui->lanjutkanButton->setEnabled(true);
            ui->hentikanButton->setEnabled(false);
            ui->batalButton->setEnabled(true);
        }
    }
}

void Form::timerTimeout()
{
    if(!m_selectionModel->hasSelection())
        return;

    if(m_selectionModel->selectedRows().count() == 0)
        return;
    QModelIndex idx = m_selectionModel->selectedRows().at(0);

    idx = m_proxyModel->mapToSource(idx);
    changeStatusButton(idx);
}

void Form::makeCenter()
{
    QSize sz = qApp->desktop()->size();
    int x = (sz.width() - width()) / 2;
    int y = (sz.height() - height()) / 2;
    move(x, y);
}

void Form::verifyDownloads()
{
    if(m_queue.count() > 0)
    {
        int curr = m_queue.dequeue();
        QModelIndex idx = m_model->index(curr, 1); // url
        Downloader *dl = m_downloaderList.dequeue();
        m_model->setData(m_model->index(curr, 3), DownloadData::Downloading); // status
        dl->download(idx);
        downloaderMap[dl] = idx;
        qDebug() << dl->objectName() << idx;
        qDebug() << "jumlah downloader sekarang:" << m_downloaderList.count();
    }
    else
    {
        // kondisi di mana objek unduhan sedang nganggur
        // semua, cek bila ada unduhan yang sedang di-paused.
        if(m_downloaderList.count() == m_maks_concurrent)
        {
            // loop untuk memastikan tidak ada unduhan yg masih PAUSED
            for(int i = 0; i < m_model->rowCount(); i++)
            {
                if(m_model->index(i, 0).data(Qt::CheckStateRole).toInt() == 0) // checked
                    continue; // abaikan index yg tidak dipilih untuk diunduh
                else {
                    // cek jika masih ada unduhan yang di-paused
                    if(m_model->index(i, 3).data(Qt::EditRole).toInt() == DownloadData::Paused) // status
                        return; // keluar dari fungsi ini
                }
            }

            // semua unduhan telah telah selesai diunduh
            while(m_downloaderList.count() > 0)
            {
                // saatnya menhancurkan objek unduhan
                QObject *obj = m_downloaderList.dequeue();
                obj->setParent(0);
                obj->deleteLater();
            }

            // hentikan timer untuk mengecek status unduhan
            m_timer->stop();
            ui->unduhButton->setEnabled(true);
            ui->cariButton->setEnabled(true);
            m_model->setCheckEnabled(true);
            ui->pilihSemuaButton->setEnabled(true);
            ui->kosongkanSemuaButton->setEnabled(true);
            ui->cariPaketLineEdit->setEnabled(true);
            ui->buttonWidget->hide();
            ui->statusbar->showMessage(tr("Semua paket telah diunduh..."));
            m_proxyModel->setHideUncheked(false);
            ui->treeView->header()->showSection(0);
        }
    }
}
