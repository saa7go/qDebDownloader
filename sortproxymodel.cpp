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
#include "sortproxymodel.h"
#include "downloadtablemodel.h"
#include <QDebug>

SortProxyModel::SortProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent), m_hideUnchecked(false)
{
}

SortProxyModel::~SortProxyModel()
{
    qDebug() << this << "destroyed!";
}

bool SortProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    const int &column = left.column();
    QVariant leftData, rightData;

    if(column == DownloadTableModel::COL_CHECHED) {
        leftData = sourceModel()->data(left, Qt::EditRole);
        rightData = sourceModel()->data(right, Qt::EditRole);
        return leftData.toInt() < rightData.toInt();
    }
    if(column == DownloadTableModel::COL_PACKAGE_NAME) {
        leftData = sourceModel()->data(left, Qt::DisplayRole);
        rightData = sourceModel()->data(right, Qt::DisplayRole);
        return leftData.toString().toAscii() < rightData.toString().toAscii();
    }
    else if(column == DownloadTableModel::COL_PROGRESS) {
        leftData = sourceModel()->data(left, Qt::EditRole);
        rightData = sourceModel()->data(right, Qt::EditRole);
        return leftData.toInt() < rightData.toInt();
    }
    else if (column == DownloadTableModel::COL_CURRENT_SIZE || column == DownloadTableModel::COL_TARGET_SIZE){ // semoga column 5 & 6
        leftData = sourceModel()->data(left, Qt::EditRole);
        rightData = sourceModel()->data(right, Qt::EditRole);
        return leftData.toLongLong() < rightData.toLongLong();
    }
    else {
        qDebug() << "ELSE!";
        return QSortFilterProxyModel::lessThan(left, right);
    }
}

bool SortProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if(m_hideUnchecked == false)
        return true;
    const QModelIndex &idx = sourceModel()->index(source_row, DownloadTableModel::COL_CHECHED, source_parent);
    return idx.data(Qt::CheckStateRole).toBool();
}

void SortProxyModel::setHideUncheked(bool val)
{
    if(m_hideUnchecked == val)
        return;
    qDebug() << "reset";
    m_hideUnchecked = val;
    invalidateFilter();
}

