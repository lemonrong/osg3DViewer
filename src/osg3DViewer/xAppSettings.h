
/******************************************************************************
   osGraphX: a 3D file viewer
   Copyright(C) 2011-2012  xbee@xbee.net

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *******************************************************************************/

#ifndef _XAPPETTINGS_H_
#define _XAPPETTINGS_H_

#include <QtCore/QSettings>

#define PACKAGE_ORGANIZATION "Lemon Rong"
#define PACKAGE_NAME "osg3DViewer"
#define PACKAGE_VERSION "1.0.0"

class xAppSettings : public QSettings
{
    Q_OBJECT

public:
    xAppSettings();
};

#endif // _APPETTINGS_H_
