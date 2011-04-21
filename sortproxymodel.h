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
#ifndef SORTPROXYMODEL_H
#define SORTPROXYMODEL_H

#include <QSortFilterProxyModel>

class SortProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit SortProxyModel(QObject *parent = 0);
    ~SortProxyModel();
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    void setHideUncheked(bool val);
    bool hideUnchecked() const { return m_hideUnchecked; }

private:
    bool m_hideUnchecked;
};

#endif // SORTPROXYMODEL_H
