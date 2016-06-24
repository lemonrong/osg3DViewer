/******************************************************************************
   osg3DViewer: a 3D file viewer
   Copyright(C) 2015-2015  ronggenmiao@163.com

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

 *******************************************************************************/
 
#ifndef _OSG3DVIEWER_SELECTIONMANAGER_H
#define _OSG3DVIEWER_SELECTIONMANAGER_H

#include <osg/Group>
#include <osg/StateSet>
#include <osg/PolygonOffset>
#include <osg/PolygonMode>
#include <osg/LineWidth>
#include <osg/Material>
#include <osg/Geometry>

class xSelectionDecorator;

class xSelectionManager
{
public:
    xSelectionManager();             //!< Constructor
    virtual ~xSelectionManager();    //!< Destructor

    // Select operations
    void clearSelection();
    bool select(osg::Node *pNode);                       //!< Select a node in the scene.
    bool select(osg::Geometry *pGeom);                   //!< Select a geometry in the scene.
    //bool select(float x, float y);                     //!< Select a point in screen coordinate.
    osg::Node* getSelectedNode();                        //!< Return the current selected node.
    osg::Geometry* getSelectedGeometry();                    //!< Return the current selected geometry.
    const osg::Node* getSelectedNode() const;            //!< Return the current selected node.
    const osg::Geometry* getSelectedGeometry() const;                    //!< Return the current selected geometry.

    // Replace and get the selection decorator
    void setSelectionDecorator(xSelectionDecorator* pDecorator);         //!< Replace the selection decorator for highlighting.
    xSelectionDecorator* getSelectionDecorator();                        //!< Return the selection decorator.
    const xSelectionDecorator* getSelectionDecorator() const;            //!< Return the selection decorator.

protected:
    osg::ref_ptr<osg::Node> m_ptrSelectedNode;    //!< Selected node
    osg::ref_ptr<osg::Geometry> m_ptrSelectedGeometry;    //!< Selected geometry
    osg::ref_ptr<xSelectionDecorator> m_ptrSelectionDecorator;    //!< Decorator node for highlighting
};

#endif // _OSG3DVIEWER_SELECTIONMANAGER_H
