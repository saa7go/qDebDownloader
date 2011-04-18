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

#include "helper.h"

void AptOffline::caculateSize(qint64 inSize, double *outSize, QString *strPostfix)
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
