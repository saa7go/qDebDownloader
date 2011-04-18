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

#ifndef DOWNLOADTABLEMODEL_H
#define DOWNLOADTABLEMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include "downloaddata.h"

class DownloadTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit DownloadTableModel(QObject *parent = 0);
    ~DownloadTableModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;

    void setUrlList(const QStringList &urlList);

    void setCheckEnabled(bool val);

private:
    QList<DownloadData> m_data;
    bool m_checkable;
};

#endif // DOWNLOADTABLEMODEL_H
