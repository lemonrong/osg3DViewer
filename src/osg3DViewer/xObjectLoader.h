
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

#ifndef _XOBJECT_LOADER_H_
#define _XOBJECT_LOADER_H_

#include <QtCore/QObject>

#include <osg/Node>

class xObjectLoader : public QObject
{
    Q_OBJECT

public:
    xObjectLoader(QObject *parent = 0);
    virtual ~xObjectLoader(){}

public slots:
    void slotNewObjectToLoad(const QString &);
    void slotSetOptimization(bool val) {m_bOptimize = val; }

signals:
    void sigNewObjectToView(osg::Node *);

private:
    QString m_file;
    osg::ref_ptr<osg::Node> m_ptrLoadedModel;
    bool m_bOptimize;
};

#endif // _SCREENSHOT_HANDLER_H_
