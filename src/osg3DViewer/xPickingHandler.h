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
 
#ifndef _XPICKINGHANDLER_H_
#define _XPICKINGHANDLER_H_

#include <osgGA/TrackballManipulator>
#include <osgGA/GUIActionAdapter>
#include <osgViewer/Viewer>

#include <QtCore/QObject>

class xPickingHandler : public QObject, public osgGA::TrackballManipulator
{
    Q_OBJECT

public:

    enum
    {
        ZOOMIN = 0,
        ZOOMOUT
    };

    xPickingHandler();

    virtual const char* className() const { return "PickingHandler"; }

    /** handle events, return true if handled, false otherwise.*/
    virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us);

    /** Get the keyboard and mouse usage of this manipulator.*/
    virtual void getUsage(osg::ApplicationUsage& usage) const;

    void pick(const osgGA::GUIEventAdapter& ea);

    void setEnabledTrackballHelper(bool val) {m_bTrackballHelper = val; }
    void setEnabledInverseMouseWheel(bool val) {m_bInverseMouseWheel = val; }

signals:
    void sigPicked(osg::Drawable *);
    void sigPicked(double, double, double);
    void sigZoomViewIn();
    void sigZoomViewOut();
    void sigDragView();
    void sigRotateView();
    void sigRecenterViewTo(double,double,double);

protected:

    virtual ~xPickingHandler();
    osg::Matrix matrixListtoSingle(osg::MatrixList tmplist);
    void zoom(int sens, int inverse);

    //viewer of the scene
    osgViewer::View* m_pViewer;

    int m_nDummy;
    bool m_bRecenter;
    bool m_bPicking;
    bool m_bTrackballHelper;
    bool m_bInverseMouseWheel;
};

#endif // _PICKINGHANDLER_H_
