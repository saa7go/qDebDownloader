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

#include "sortproxymodel.h"
#include <QDebug>

SortProxyModel::SortProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
}

SortProxyModel::~SortProxyModel()
{
    qDebug() << this << "destroyed!";
}

bool SortProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    int column = left.column();
    QVariant leftData, rightData;

    if(column == 1) {
        leftData = sourceModel()->data(left, Qt::DisplayRole);
        rightData = sourceModel()->data(right, Qt::DisplayRole);
        return leftData.toString().toAscii() < rightData.toString().toAscii();
    }
    else if(column == 3) {
        leftData = sourceModel()->data(left, Qt::EditRole);
        rightData = sourceModel()->data(right, Qt::EditRole);
        return leftData.toInt() < rightData.toInt();
    }
    else if (column == 4 || column == 5){ // semoga column 4 & 5
        leftData = sourceModel()->data(left, Qt::EditRole);
        rightData = sourceModel()->data(right, Qt::EditRole);
        return leftData.toLongLong() < rightData.toLongLong();
    }
    else {
        qDebug() << "ELSE!";
        return QSortFilterProxyModel::lessThan(left, right);
    }
}
