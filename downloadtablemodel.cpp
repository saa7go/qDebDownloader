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
    return parent.isValid() ? 0 : 6;
}

QVariant DownloadTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        DownloadData data = m_data.at(index.row());
        if(index.column() == 0)
            return data.url();
        else if(index.column() == 1)
            return data.packageName();
        else if(index.column() == 2)
            return data.status();
        else if(index.column() == 3)
            return data.progress();
        else if(index.column() == 4)
        {
            if(role == Qt::EditRole)
                return data.size();
            else {
                if(m_error.at(index.row()).isEmpty())
                {
                    double dSize;
                    QString ukuran;
                    AptOffline::caculateSize(data.size(), &dSize, &ukuran);
                    return QString("%1 %2").arg(QString::number(dSize, 'f', 2)).arg(ukuran);
                }
                else {
                    return m_error.at(index.row());
                }
            }
        }
        else if(index.column() == 5)
        {
            if(role == Qt::EditRole)
                return data.fileSize();
            else
            {
                double dSize;
                QString ukuran;
                AptOffline::caculateSize(data.fileSize(), &dSize, &ukuran);
                return QString("%1 %2").arg(QString::number(dSize, 'f', 2)).arg(ukuran);
            }
        }
    }
    if(role == Qt::CheckStateRole)
    {
        if(index.column() == 1)
        {
            if(m_data.at(index.row()).selected())
                return Qt::Checked;
            else
                return Qt::Unchecked;
        }
    }
    if(role == Qt::TextAlignmentRole)
    {
        if(index.column() == 4)
            return Qt::AlignCenter;
        if(index.column() == 5)
        {
            Qt::Alignment fl = Qt::AlignRight | Qt::AlignVCenter;
            return (int)fl;
        }
    }

#ifdef Q_WS_WIN
    if(role == Qt::FontRole) {
        QFont _font = qApp->font();
        _font.setBold(true);
        return _font;
    }
#endif
    return QVariant();
}

bool DownloadTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid())
        return false;
    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        int row = index.row(), col = index.column();
        if(col == 0)
        {
            if(m_data[row].url() != value.toString())
            {
                m_data[row].setUrl(value.toString());
                emit dataChanged(index, index);
            }
        }
        else if(col == 1)
        {
            if(m_data.at(row).packageName() != value.toString())
            {
                m_data[row].setPackageName(value.toString());
                emit dataChanged(index, index);
            }
        }
        else if(col ==2)
        {
            if(m_data.at(row).status() != value.toInt())
            {
                QModelIndex rightIndex = this->index(row, col + 1);
                m_data[row].setStatus(value.toInt());
                emit dataChanged(index, rightIndex);
            }
        }
        else if(col == 3)
        {
            if(m_data.at(row).progress() != value.toInt())
            {
                m_data[row].setProgress(value.toInt());
                emit dataChanged(index, index);
            }
        }
        else if(col == 4)
        {
            if(role == Qt::DisplayRole)
            {
                m_error[row] = value.toString();
                emit dataChanged(index, index);
            }
            else
            {
                if(m_data.at(row).size() != value.toLongLong())
                {
                    m_data[row].setSize(value.toLongLong());
                    emit dataChanged(index, index);
                }
            }
        }
        else if(col == 5)
        {
            if(m_data.at(row).fileSize() != value.toLongLong())
            {
                m_data[row].setFileSize(value.toLongLong());
                emit dataChanged(index, index);
            }
        }
        else
            return false;
    }
    if(role == Qt::CheckStateRole)
    {
        if(index.column() == 1)
        {
            int  _state = value.toInt();
            bool isChecked;
            _state == 0 ? isChecked = false : isChecked = true;
            if(m_data.at(index.row()).selected() != isChecked)
            {
                m_data[index.row()].setSelected(isChecked);
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
        if(index.column() == 1)
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
            if(section == 0)
                return tr("Url");
            else if(section == 1)
                return tr("Nama file");
            else if(section == 2)
                return tr("Status");
            else if(section == 3)
                return tr("Progres Unduhan");
            else if(section == 4)
                return tr("Ukuran Terunduh");
            else if(section == 5)
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
    emit dataChanged(index(0, 1), index(row, 1));
}
