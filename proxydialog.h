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

#ifndef PROXYDIALOG_H
#define PROXYDIALOG_H

#include <QDialog>
#include <QNetworkProxy>

namespace Ui {
    class ProxyDialog;
}

class ProxyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProxyDialog(QWidget *parent = 0);
    ~ProxyDialog();

    QNetworkProxy proxy() const { return m_proxy; }

private Q_SLOTS:
    void acceptProxy();
    void proxyIndexListChanged(int index);

private:
    Ui::ProxyDialog *ui;
    QNetworkProxy m_proxy;

    void readSetting();
    void writeSetting();

};

#endif // PROXYDIALOG_H