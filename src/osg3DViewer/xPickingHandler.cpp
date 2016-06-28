
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
 
#include "xPickingHandler.h"

#include <osg/Notify>
#include <osgUtil/IntersectionVisitor>
#include <osgUtil/PolytopeIntersector>
#include <osgUtil/LineSegmentIntersector>

using namespace osg;
using namespace osgGA;

#include <iostream>

xPickingHandler::xPickingHandler() :
    TrackballManipulator(),
    m_viewer(NULL),
    m_dummy(0),
    m_isRecenter(false),
    m_isPicking(false),
    m_isTrackballHelper(false),
    m_isInverseMouseWheel(false)
{}

xPickingHandler::~xPickingHandler()
{}

osg::Matrix xPickingHandler::matrixListtoSingle(osg::MatrixList tmplist)
{
    osg::Matrix tmp;

    if (tmplist.size() > 0)
    {
        size_t i;
        for (i = 1, tmp = tmplist[0]; i < tmplist.size(); i++)
            tmp *= tmplist[0];
        tmp = tmplist[0];
    }
    return (tmp);
}

void xPickingHandler::getUsage(osg::ApplicationUsage& usage) const
{
    usage.addKeyboardMouseBinding("PickingHandler: Space","Reset the viewing position to home");
    usage.addKeyboardMouseBinding("PickingHandler: Shift","Clic to center the view under the cursor (usefull to turn around a object)");
    TrackballManipulator::getUsage(usage);
}

bool xPickingHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us)
{
    bool handled = false;
    m_viewer = dynamic_cast<osgViewer::View*>(&us);
    if (!m_viewer)
        return false;

    switch(ea.getEventType())
    {
        case osgGA::GUIEventAdapter::PUSH:
        {
            if (ea.getButtonMask()==osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
                pick(ea);
            break;
        }

        case osgGA::GUIEventAdapter::DRAG:
        {
            if (ea.getButtonMask()==osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
            {
                pick(ea);
                if (m_isTrackballHelper)
                    emit sigRotateView();
            }
            else if (ea.getButtonMask()==osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON && m_isTrackballHelper)
                emit sigDragView();
            else if (ea.getButtonMask()==osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON && m_isTrackballHelper)
                emit sigZoomViewIn();
            break;
        }

        case GUIEventAdapter::SCROLL:
        {
            switch (ea.getScrollingMotion())
            {
                case GUIEventAdapter::SCROLL_UP:

                    //handle scroll down;
                    zoom(ZOOMIN,m_isInverseMouseWheel);
                    handled = true;
                    break;
                case GUIEventAdapter::SCROLL_DOWN:

                    //handle scroll up
                    zoom(ZOOMOUT,m_isInverseMouseWheel);
                    handled = true;
                    break;
                default:

                    break;
            }
            break;
        }

        case GUIEventAdapter::KEYDOWN:
        {
            if (ea.getKey() == GUIEventAdapter::KEY_Shift_L || ea.getKey() == GUIEventAdapter::KEY_Shift_R)
            {
                m_isRecenter = true;
                handled = true;
            }
            else if (ea.getKey() == GUIEventAdapter::KEY_Control_L || ea.getKey() == GUIEventAdapter::KEY_Control_R)
            {
                m_isPicking = true;
                handled = true;
            }
            else if (ea.getKey()== GUIEventAdapter::KEY_Space)
            {
                home(ea,us);
                us.requestRedraw();
                return true;
            }

            break;
        }

        case (GUIEventAdapter::KEYUP):
        {
            m_isRecenter = false;
            m_isPicking = false;
			
            if (ea.getKey() == GUIEventAdapter::KEY_Shift_L || ea.getKey() == GUIEventAdapter::KEY_Shift_R)
            {
                m_isRecenter = false;
                handled = true;
            }
            else if (ea.getKey() == GUIEventAdapter::KEY_Control_L || ea.getKey() == GUIEventAdapter::KEY_Control_R)
            {
                m_isPicking = false;
                handled = true;
            }
            break;
        }
        default:
        {
        }
    }

    if (handled)
        return true;
    else
        return TrackballManipulator::handle(ea, us);
}

void xPickingHandler::zoom(int sens, int inverse)
{
    if (inverse)
    {
        if (sens == ZOOMIN)
            sens = ZOOMOUT;
        else
            sens = ZOOMIN;
    }

    if (sens == ZOOMIN)
    {
        _distance *= 1.2;
        if (_distance > 1e+7)
            _distance = 1e+7;

        if (m_isTrackballHelper)
            emit sigZoomViewIn();
    }
    else
    {
        _distance *= 0.8;
        if (_distance < 0.05)
            _distance = 0.05;

        if (m_isTrackballHelper)
            emit sigZoomViewOut();
    }
}

void xPickingHandler::pick(const osgGA::GUIEventAdapter& ea)
{
    if (!m_isRecenter && !m_isPicking)
        return;

    osg::Node* scene = m_viewer->getSceneData();
    if (!scene)
        return;

    osg::Node* node = 0;
    osg::Group* parent = 0;

    bool usePolytopePicking = false;
    if (usePolytopePicking)
    {
        double mx = ea.getXnormalized();
        double my = ea.getYnormalized();
        double w = 0.05;
        double h = 0.05;
        osgUtil::PolytopeIntersector* picker = new osgUtil::PolytopeIntersector(osgUtil::Intersector::PROJECTION, mx - w, my - h, mx + w, my + h);

        osgUtil::IntersectionVisitor iv(picker);

        m_viewer->getCamera()->accept(iv);

        if (picker->containsIntersections())
        {
            osgUtil::PolytopeIntersector::Intersection intersection = picker->getFirstIntersection();

            osg::NodePath& nodePath = intersection.nodePath;
            node = (nodePath.size()>=1) ? nodePath[nodePath.size() - 1] : 0;
            parent = (nodePath.size()>=2) ? dynamic_cast<osg::Group*>(nodePath[nodePath.size() - 2]) : 0;
        }
    }
    else
    {
        // use window coordinates
        // remap the mouse x,y into viewport coordinates.
        osg::Viewport* viewport = m_viewer->getCamera()->getViewport();
        float mx = viewport->x() + (int)((float)viewport->width() * (ea.getXnormalized() * 0.5f + 0.5f));
        float my = viewport->y() + (int)((float)viewport->height() * (ea.getYnormalized() * 0.5f + 0.5f));
        osgUtil::LineSegmentIntersector* picker = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, mx, my);

        osgUtil::IntersectionVisitor iv(picker);

        m_viewer->getCamera()->accept(iv);

        if (picker->containsIntersections())
        {
            osgUtil::LineSegmentIntersector::Intersection intersection = picker->getFirstIntersection();

            if (m_isRecenter)
            {
                osg::NodePath& nodePath = intersection.nodePath;
                node = (nodePath.size()>=1) ? nodePath[nodePath.size() - 1] : 0;

                // world matrix transform
                osg::Matrix mat = matrixListtoSingle(node->getWorldMatrices());

                _center = intersection.localIntersectionPoint * mat;

                return;
            }

            osg::NodePath& nodePath = intersection.nodePath;
            node = (nodePath.size()>=1) ? nodePath[nodePath.size() - 1] : 0;

            emit sigPicked(intersection.drawable.get());
            parent = (nodePath.size()>=2) ? dynamic_cast<osg::Group*>(nodePath[nodePath.size() - 2]) : 0;
        }
    }
}
