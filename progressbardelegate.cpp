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

#include "progressbardelegate.h"
#include <QPainter>
#include <QApplication>
#include <QDebug>
#include "downloaddata.h"

ProgressBarDelegate::ProgressBarDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

ProgressBarDelegate::~ProgressBarDelegate()
{
    qDebug() << this << "destroyed";
}

void ProgressBarDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // progress = 3ef100
    // finished = 2eb100
    // error = ff0000
    // paused = fffd5a
    // waiting = grey
    QRect rect = option.rect;
    QPoint topLeft = option.rect.topLeft();
    rect.setWidth(option.rect.width() - 2);
    rect.setHeight(option.rect.height() - 2);
    topLeft.setX(option.rect.x() + 2);
    topLeft.setY(option.rect.y()  + 2);
    rect.setTopLeft(topLeft);

    int width = rect.width();
    width = (index.data(Qt::EditRole).toDouble() / 100.0) * width;
    rect.setWidth(width);


    QColor col = Qt::transparent;
    int status = index.model()->index(index.row(), 2).data(Qt::EditRole).toInt();

    if(status == DownloadData::Downloading)
        col.setNamedColor("#3ef100");
    else if(status == DownloadData::Finished)
        col.setNamedColor("#2eb100");
    else if(status == DownloadData::Paused)
        col.setNamedColor("#fffd5a");
    else if(status == DownloadData::Canceled)
        col.setNamedColor("#ff0000");
    else if(status == DownloadData::Waiting)
        col = Qt::gray;
    painter->save();
    painter->setBrush(QBrush(col));
    painter->setPen(Qt::NoPen);
    painter->drawRect(rect);
    painter->restore();


    // menggunakan native progresbar
    /*
    QStyleOptionProgressBarV2 opt;
    opt.rect = option.rect;
    opt.minimum = 0;
    opt.maximum = 100;
    opt.progress = index.data(Qt::EditRole).toInt();
    opt.text = QString::number(opt.progress) + "%";
    opt.textVisible = true;
    opt.textAlignment = Qt::AlignCenter;
    QApplication::style()->drawControl(QStyle::CE_ProgressBar, &opt, painter);
    if(option.state & QStyle::State_Selected)
    {
        painter->save();
        QPen pen(Qt::SolidLine);
        pen.setWidth(2);
        pen.setColor(option.palette.highlight().color());
        QRect rect = option.rect;
        rect.setX(rect.x() + 1);
        rect.setY(rect.y() + 1);
        rect.setWidth(rect.width() - 1);
        rect.setHeight(rect.height() - 1);
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(rect);
        painter->restore();
    }
    if(option.state && QStyle::State_Selected)
    {
        painter->save();
        painter->setPen(Qt::NoPen);
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(option.rect);
        painter->restore();
    }
    */
}
