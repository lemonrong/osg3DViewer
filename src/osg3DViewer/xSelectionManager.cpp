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

 
#include "xSelectionManager.h"
#include "xSelectionDecorator.h"

#include <osgDB/Registry>
#include <osgDB/FileUtils>

//#include "OsgData.h"

xSelectionManager::xSelectionManager()
{
    // create a selection decorator for highlighting.
    m_ptrSelectionDecorator = new xDefaultSelectionDecorator;
}

/*!
    Destructor
 */
xSelectionManager::~xSelectionManager()
{}

void xSelectionManager::clearSelection()
{
    // deselect the previous selected node
    while (m_ptrSelectionDecorator->getNumChildren() > 0)
        m_ptrSelectionDecorator->removeChild(0,1);

    if (m_ptrSelectionDecorator->getNumParents() > 0)
        m_ptrSelectionDecorator->getParent(0)->removeChild(m_ptrSelectionDecorator.get());
}

/*!
    Select a node in the scene.
    @param[in] pNode Node to be selected (or NULL to deselect).
    @return Return 'true' if the node is selected. Return 'false' when deselected.
 */
bool xSelectionManager::select(osg::Node* pNode)
{
    // select the new node
    if (pNode)
    {
        m_ptrSelectionDecorator->addChild(pNode);

        osg::Group* pParent = pNode->getParent(0);

        if (pParent)
        {
            pParent->addChild(m_ptrSelectionDecorator.get());
        }
    }

    // keep the pointer of the selected node
    m_ptrSelectedNode = pNode;

    return (pNode!=NULL);
}

/*!
    Select a geometry element in the scene.
    @param[in] pGeom geometry to be selected (or NULL to deselect).
    @return Return 'true' if the geometry is selected. Return 'false' when deselected.
 */
bool xSelectionManager::select(osg::Geometry* pGeom)
{
    // select the new node
    if (pGeom)
    {
        // creation of a dummy geode
        //osg::Geode *newparent = dynamic_cast<osg::Geode*>(pGeom->getParent(0)->clone(osg::CopyOp::DEEP_COPY_STATESETS));
        osg::Geode *newparent = new osg::Geode;

        // duplicate current geometry
        osg::Geometry *newgeom = dynamic_cast<osg::Geometry*>(pGeom->clone(osg::CopyOp::DEEP_COPY_PRIMITIVES));

        newparent->addDrawable(newgeom);

        m_ptrSelectionDecorator->addChild(newparent);

        osg::Node* pParent = pGeom->getParent(0);

        if (pParent)
            pParent->getParent(0)->addChild(m_ptrSelectionDecorator.get());
    }

    // keep the pointer of the selected node
    m_ptrSelectedGeometry = pGeom;

    return (pGeom!=NULL);
}

/*!
    Return the current selected node.
    @return Current selected node.
 */
osg::Node* xSelectionManager::getSelectedNode()
{
    return m_ptrSelectedNode.get();
}

/*!
    Return the current selected geometry.
    @return Current selected geometry.
 */
osg::Geometry* xSelectionManager::getSelectedGeometry()
{
    return m_ptrSelectedGeometry.get();
}

/*!
    Return the current selected node.
    @return Current selected node.
 */
const osg::Node* xSelectionManager::getSelectedNode() const
{
    return m_ptrSelectedNode.get();
}

/*!
    Return the current selected geometry.
    @return Current selected geometry.
 */
const osg::Geometry* xSelectionManager::getSelectedGeometry() const
{
    return m_ptrSelectedGeometry.get();
}

/*!
    Replace the selection decorator for highlighting.
    @param[in] pDecorator New selection decorator.
 */
void xSelectionManager::setSelectionDecorator(xSelectionDecorator* pDecorator)
{
    if (!pDecorator)
        return;

    m_ptrSelectionDecorator = pDecorator;
    select(m_ptrSelectedNode.get());
}

/*!
    Return the selection decorator.
    @return Current selection decorator.
 */
xSelectionDecorator* xSelectionManager::getSelectionDecorator()
{
    return m_ptrSelectionDecorator.get();
}

/*!
    Return the selection decorator.
    @return Current selection decorator.
 */
const xSelectionDecorator* xSelectionManager::getSelectionDecorator() const
{
    return m_ptrSelectionDecorator.get();
}
