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
#ifndef PROGRESSBARDELEGATE_H
#define PROGRESSBARDELEGATE_H

#include <QStyledItemDelegate>
#include <QPixmap>

class ProgressBarDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ProgressBarDelegate(QObject *parent = 0);
    ~ProgressBarDelegate();
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
#ifdef Q_OS_WIN
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
#endif

private:
    QPixmap m_downloadPix;
    QPixmap m_pausePix;
    QPixmap m_cancelPix;
    QPixmap m_waitingPix;
    QPixmap m_finishedPix;
    bool m_scaled;
};

#endif // PROGRESSBARDELEGATE_H
