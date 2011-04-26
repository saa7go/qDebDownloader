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
#include "downloadtablemodel.h"
#include <QStringList>
#include <QUrl>
#include <QDebug>
#include <QApplication>
#include <QFont>
#include "helper.h"

DownloadTableModel::DownloadTableModel(QObject *parent) :
    QAbstractTableModel(parent), m_checkable(true)
{
}

DownloadTableModel::~DownloadTableModel()
{
    qDebug() << this << "Destroyed";
}

int DownloadTableModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_data.count();
}

int DownloadTableModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 7;
}

QVariant DownloadTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    const int &row = index.row(), &col = index.column();
    const DownloadData &data = m_data.at(row);
    const QString &error = m_error.at(row);

    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if(index.column() == COL_CHECHED)
        {
            if(role == Qt::DisplayRole)
                return QString();
            else
                data.selected();
        }
        else if(col == COL_URL)
            return data.url();
        else if(col == COL_PACKAGE_NAME)
            return data.packageName();
        else if(col == COL_STATUS)
            return data.status();
        else if(col == COL_PROGRESS)
            return data.progress();
        else if(col == COL_CURRENT_SIZE)
        {
            if(role == Qt::EditRole)
                return data.size();
            else {
                if(error.isEmpty())
                {
                    double dSize;
                    QString ukuran;
                    qDebDownloader::caculateSize(data.size(), &dSize, &ukuran);
                    return QString("%1 %2").arg(QString::number(dSize, 'f', 2)).arg(ukuran);
                }
                else {
                    return error;
                }
            }
        }
        else if(col == COL_TARGET_SIZE)
        {
            if(role == Qt::EditRole)
                return data.fileSize();
            else
            {
                double dSize;
                QString ukuran;
                qDebDownloader::caculateSize(data.fileSize(), &dSize, &ukuran);
                return QString("%1 %2").arg(QString::number(dSize, 'f', 2)).arg(ukuran);
            }
        }
    }
    if(role == Qt::CheckStateRole)
    {
        if(col == COL_CHECHED)
        {
            if(data.selected())
                return Qt::Checked;
            else
                return Qt::Unchecked;
        }
    }
    if(role == Qt::TextAlignmentRole)
    {
        if(col == COL_CURRENT_SIZE)
            return Qt::AlignCenter;
        if(col == COL_TARGET_SIZE)
        {
            Qt::Alignment fl = Qt::AlignRight | Qt::AlignVCenter;
            return (int)fl;
        }
    }

#ifdef Q_WS_WIN
    if(qDebDownloader::isBeforeWinVista())
    {
        if(role == Qt::FontRole) {
            QFont _font = qApp->font();
            _font.setBold(true);
            return _font;
        }
    }
#endif
    return QVariant();
}

bool DownloadTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid())
        return false;

    const int &row = index.row(), &col = index.column();
    DownloadData &data = m_data[row];
    QString &error = m_error[row];

    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if(col == COL_URL)
        {
            if(data.url() != value.toString())
            {
                data.setUrl(value.toString());
                emit dataChanged(index, index);
            }
        }
        else if(col == COL_PACKAGE_NAME)
        {
            if(data.packageName() != value.toString())
            {
                data.setPackageName(value.toString());
                emit dataChanged(index, index);
            }
        }
        else if(col == COL_STATUS)
        {
            if(data.status() != value.toInt())
            {
                QModelIndex rightIndex = this->index(row, col + 1);
                data.setStatus(value.toInt());
                emit dataChanged(index, rightIndex);
            }
        }
        else if(col == COL_PROGRESS)
        {
            if(data.progress() != value.toInt())
            {
                data.setProgress(value.toInt());
                emit dataChanged(index, index);
            }
        }
        else if(col == COL_CURRENT_SIZE)
        {
            if(role == Qt::DisplayRole)
            {
                error = value.toString();
                emit dataChanged(index, index);
            }
            else
            {
                if(data.size() != value.toLongLong())
                {
                    data.setSize(value.toLongLong());
                    emit dataChanged(index, index);
                }
            }
        }
        else if(col == COL_TARGET_SIZE)
        {
            if(data.fileSize() != value.toLongLong())
            {
                data.setFileSize(value.toLongLong());
                emit dataChanged(index, index);
            }
        }
        else
            return false;
    }
    if(role == Qt::CheckStateRole)
    {
        if(col == COL_CHECHED)
        {
            int  _state = value.toInt();
            bool isChecked;
            _state == 0 ? isChecked = false : isChecked = true;
            if(data.selected() != isChecked)
            {
                data.setSelected(isChecked);
                emit dataChanged(index, index);
            }
        }
        else
            return false;
    }
    return true;
}

Qt::ItemFlags DownloadTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags _flags =  Qt::ItemIsSelectable;

    if(index.isValid())
    {
        if(index.column() == COL_CHECHED)
        {
            _flags |= Qt::ItemIsUserCheckable;
            if(m_checkable)
                _flags |= Qt::ItemIsEnabled;
        }
        else {
            _flags |= Qt::ItemIsEnabled;
        }
    }

    return _flags;
}

void DownloadTableModel::setUrlList(const QStringList &urlList)
{
    m_data.clear();
    m_error.clear();

    for(int i = 0; i < urlList.count(); i++)
    {
        m_data.append(DownloadData(urlList.at(i)));
        m_error << QString();
    }

    emit reset();
}

QVariant DownloadTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole)
    {
        if(orientation == Qt::Horizontal)
        {
            if(section == COL_CHECHED)
                return QString();
            else if(section == COL_URL)
                return tr("Url");
            else if(section == COL_PACKAGE_NAME)
                return tr("Nama file");
            else if(section == COL_STATUS)
                return tr("Status");
            else if(section == COL_PROGRESS)
                return tr("Progres Unduhan");
            else if(section == COL_CURRENT_SIZE)
                return tr("Ukuran Terunduh");
            else if(section == COL_TARGET_SIZE)
                return tr("Ukuran File");
        }
        else {
            return QString("%1").arg(section + 1);
        }
    }
    return QVariant();
}

void DownloadTableModel::setCheckEnabled(bool val)
{
    // men-disable kolom nomor 1 (kolom dimulai dari 0)
    m_checkable = val;
    int row = rowCount() - 1;
    emit dataChanged(index(0, 0), index(row, 0));
}
