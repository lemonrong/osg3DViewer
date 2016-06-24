
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
#include <QtCore/QFileInfo>

#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>

#include "xObjectLoader.h"
#include "xLogHandler.h"

#ifdef WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

//---------------------------------------------------------------------------------

xObjectLoader::xObjectLoader(QObject *parent) : QObject(parent), m_bOptimize(false)
{}

void xObjectLoader::slotNewObjectToLoad(const QString &file)
{
    m_file = file;

    // go to file path (for relative included resources)
    chdir(QFileInfo(m_file).absolutePath().toAscii());

    // load the scene.
    xLogHandler::getInstance()->slotReportInfo(tr("Loading of %1 ...").arg(file));
    m_ptrLoadedModel = osgDB::readNodeFile(file.toStdString());

    if (!m_ptrLoadedModel)
    {
        QString mess = tr("Error loading file %1").arg(file);
        xLogHandler::getInstance()->slotReportError(mess);
    }

    // optimize the scene graph, remove redundant nodes and state etc.
    if (m_bOptimize)
    {
        xLogHandler::getInstance()->slotReportInfo(tr("Optimization of the graph ..."));
        osgUtil::Optimizer optimizer;
        optimizer.optimize(m_ptrLoadedModel.get());
    }

    emit sigNewObjectToView(m_ptrLoadedModel);
}
