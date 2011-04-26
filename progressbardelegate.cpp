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
#include "progressbardelegate.h"
#include <QPainter>
#include <QApplication>
#include <QDebug>
#include "downloaddata.h"
#include "downloadtablemodel.h"

ProgressBarDelegate::ProgressBarDelegate(QObject *parent) :
    QStyledItemDelegate(parent), m_scaled(false)
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

    painter->save();

    const int &status = index.model()->index(index.row(), DownloadTableModel::COL_STATUS).data(Qt::EditRole).toInt();

    // menggambar highlight
    if(option.state & QStyle::State_Selected)
    {
        QColor hightLightColor;
        if(option.state & QStyle::State_Active)
            hightLightColor = QApplication::palette().color(QPalette::Highlight);
        else
            hightLightColor = QApplication::palette().color(QPalette::Inactive, QPalette::Highlight);

        painter->setBrush(hightLightColor);
        painter->setPen(Qt::NoPen);
        painter->drawRect(option.rect);
    }
    // menggambar icon di sebelah kiri progressbar
    if(!m_scaled) {
        m_iconSizeHint = sizeHint(option, index);
        m_iconSizeHint.setWidth(m_iconSizeHint.height());
        m_pausePix = QPixmap(":/pause.png").scaled(m_iconSizeHint);
        m_downloadPix = QPixmap(":/start.png").scaled(m_iconSizeHint);
        m_cancelPix = QPixmap(":/stop.png").scaled(m_iconSizeHint);
        m_waitingPix = QPixmap(":/waiting.png").scaled(m_iconSizeHint);
        m_finishedPix = QPixmap(":/finished.png").scaled(m_iconSizeHint);
        m_scaled = true;
    }

    QPixmap statusPix;
    if(status == DownloadData::Nothing || status == DownloadData::Waiting)
        statusPix = m_waitingPix;
    else if(status == DownloadData::Downloading)
        statusPix = m_downloadPix;
    else if(status == DownloadData::Paused)
        statusPix = m_pausePix;
    else if(status == DownloadData::Finished)
        statusPix = m_finishedPix;
    else // DownloadData::Canceled
        statusPix = m_cancelPix;
    painter->drawPixmap(option.rect.x(), option.rect.y(), m_iconSizeHint.width(), m_iconSizeHint.height(), statusPix);

    // menggambar background progressbar
    QRect _backgrounRect = option.rect;
    _backgrounRect.adjust(m_iconSizeHint.width() + 2, 2, -2, -2);
    QPen _pen = Qt::NoPen;
    QBrush _brush(QApplication::palette().color(QPalette::Window));
    painter->setPen(_pen);
    painter->setBrush(_brush);
    painter->drawRect(_backgrounRect);

    QColor backgrnd = QApplication::palette().color(QPalette::Window);
    backgrnd = backgrnd.darker();
    _pen.setColor(backgrnd);
    _pen.setStyle(Qt::SolidLine);
    _pen.setWidth(1);
    painter->setPen(_pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawLine(_backgrounRect.topLeft(), _backgrounRect.topRight());
    painter->drawLine(_backgrounRect.topLeft(), _backgrounRect.bottomLeft());


    // menggambar progress yang sedang berjalan
    QRect rect = option.rect;
    rect.adjust(m_iconSizeHint.width() + 4, 4, -4, -4);

    int width = rect.width();
    width = (index.data(Qt::EditRole).toDouble() / 100.0) * width;
    rect.setWidth(width);

    QColor col = Qt::transparent;

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

#ifdef Q_OS_WIN
QSize ProgressBarDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize tmp = QStyledItemDelegate::sizeHint(option, index);
    int height = tmp.height();
    tmp.setHeight(height+3);
    return tmp;
}
#endif
