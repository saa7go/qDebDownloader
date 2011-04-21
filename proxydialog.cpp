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
#include "proxydialog.h"
#include "ui_proxydialog.h"
#include <QSettings>
#include <QValidator>

ProxyDialog::ProxyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProxyDialog)
{
    ui->setupUi(this);

    ui->proxyListComboBox->clear();
    ui->proxyListComboBox->addItem(tr("No Proxy"));
    ui->proxyListComboBox->addItem(tr("Default Proxy"));
    ui->proxyListComboBox->addItem(tr("Http Proxy"));
    ui->proxyListComboBox->addItem(tr("Socks5 Proxy"));

    ui->portEdit->setValidator(new QIntValidator(0, 65535, this));

    connect(ui->okButton, SIGNAL(clicked()), SLOT(acceptProxy()));
    connect(ui->okButton, SIGNAL(clicked()), SLOT(accept()));
    connect(ui->cancelButton, SIGNAL(clicked()), SLOT(reject()));
    connect(ui->proxyListComboBox, SIGNAL(currentIndexChanged(int)), SLOT(proxyIndexListChanged(int)));

    ui->frame->setVisible(false);

    readSetting();
}

ProxyDialog::~ProxyDialog()
{
    delete ui;
}

void ProxyDialog::acceptProxy()
{
    m_proxy = QNetworkProxy(); // reset

    int index = ui->proxyListComboBox->currentIndex();
    if(index == 0)
        m_proxy.setType(QNetworkProxy::NoProxy);
    else if(index == 1)
        m_proxy.setType(QNetworkProxy::DefaultProxy);
    else {
        if(index ==  2)
            m_proxy.setType(QNetworkProxy::HttpProxy);
        else if(index == 3)
            m_proxy.setType(QNetworkProxy::Socks5Proxy);
        else
            return;

        m_proxy.setHostName(ui->hostnameEdit->text());
        m_proxy.setPort(ui->portEdit->text().toInt());
        m_proxy.setUser(ui->usernameEdit->text());
        m_proxy.setPassword(ui->passwordEdit->text());
    }

    writeSetting();
}

void ProxyDialog::proxyIndexListChanged(int index)
{
    switch(index)
    {
    case 0:
    case 1:
        ui->frame->setVisible(false);
        break;
    case 2:
    case 3:
        ui->frame->setVisible(true);
        break;
    }
}

void ProxyDialog::readSetting()
{
    QSettings setting("./apt-web.ini", QSettings::IniFormat);
    int type = setting.value("Proxy/Type", 0).toInt();
    if(type < 0 || type > 3)
        type = 0;

    ui->proxyListComboBox->setCurrentIndex(type);

    if(type == 2 || type == 3){
        QString host = setting.value("Proxy/Hostname", "").toString();
        int port = setting.value("Proxy/Port", 0).toInt();
        QString username = setting.value("Proxy/Username", "").toString();
        QString password = setting.value("Proxy/Password", "").toString();

        ui->hostnameEdit->setText(host);
        ui->portEdit->setText(port == 0 ? "" : QString::number(port));
        ui->usernameEdit->setText(username);
        ui->passwordEdit->setText(password);
    }
}

void ProxyDialog::writeSetting()
{
    QSettings setting("./apt-web.ini", QSettings::IniFormat);
    int index = ui->proxyListComboBox->currentIndex();
    setting.setValue("Proxy/Type", index);

    int port = 0;
    QString host = "", username = "", password = "";

    if(index == 2 || index == 3){
        host = ui->hostnameEdit->text();
        port = ui->portEdit->text().toInt();
        username = ui->usernameEdit->text();
        password = ui->passwordEdit->text();
    }

    setting.setValue("Proxy/Hostname", host);
    setting.setValue("Proxy/Port", port);
    setting.setValue("Proxy/Username", username);
    setting.setValue("Proxy/Password", password);
}
