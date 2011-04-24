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
#include "helper.h"
#ifdef Q_OS_WIN
#include <windows.h>
#endif
void qDebDownloader::caculateSize(qint64 inSize, double *outSize, QString *strPostfix)
{
    *outSize = inSize;

    if(inSize / 1048576 > 0)
    {
        *strPostfix = "MiB";
        *outSize /= 1048576.0;
    }
    else if(inSize / 1024 > 0)
    {
        *strPostfix = "KiB";
        *outSize /= 1024.0;
    }
    else
        *strPostfix = "B";
}

#ifdef Q_OS_WIN
    bool qDebDownloader::isBeforeWinVista()
    {
        static bool isSet = false;
        static bool beforeWinVista;

        if(!isSet)
        {
            OSVERSIONINFO osVerInfo;

            ZeroMemory(&osVerInfo, sizeof(OSVERSIONINFO));
            osVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

            GetVersionEx(&osVerInfo);
            beforeWinVista = osVerInfo.dwMajorVersion < 6 ? true : false;
            isSet = true;
        }
        return beforeWinVista;
    }

#endif
