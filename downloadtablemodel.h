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
#ifndef DOWNLOADTABLEMODEL_H
#define DOWNLOADTABLEMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QStringList>
#include "downloaddata.h"

class DownloadTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum COLUMN_NAME {
        COL_CHECHED = 0,
        COL_URL = 1,
        COL_PACKAGE_NAME = 2,
        COL_STATUS = 3,
        COL_PROGRESS = 4,
        COL_CURRENT_SIZE = 5,
        COL_TARGET_SIZE = 6
    };

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
    QStringList m_error;
    bool m_checkable;
};

#endif // DOWNLOADTABLEMODEL_H
