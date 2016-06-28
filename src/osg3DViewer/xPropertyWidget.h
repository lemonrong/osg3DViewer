
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

#ifndef _XPROPERTYWIDGET_H_
#define _XPROPERTYWIDGET_H_

#include <QtCore/QString>
#include <QtGui/QScrollArea>
#include <QtCore/QMap>

#include <osg/Node>
#include <osg/Switch>
#include <osg/LOD>
#include <osg/Geode>

#include "qttreepropertybrowser.h"
#include "qtvariantproperty.h"

class xPropertyWidget : public QtTreePropertyBrowser
{
    Q_OBJECT

public:

    xPropertyWidget(QWidget *parent = 0);
    ~xPropertyWidget() {}

    void displayProperties(osg::Node *);
    void displayNodeProperties(osg::Node *node);
    void displayLODProperties(osg::LOD *);
    void displaySwitchProperties(osg::Switch *);
    void displayGeodeProperties(osg::Geode *node);
    void displayBaseStats(osg::Node *);

public slots:

signals:

protected:

private:
    void initDictionaries();

    QString m_file;

    QtVariantPropertyManager *m_variantManager;

    QList<QString> m_dataVariances;
    QList<QString> m_centerModes;
    QList<QString> m_rangeModes;
};

#endif // _PROPERTYWIDGET_H_
