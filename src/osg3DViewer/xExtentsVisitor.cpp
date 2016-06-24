
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

#include "xExtentsVisitor.h"

// handle geode drawable extents to expand the box
//------------------------------------------------------------------------
// apply.
//------------------------------------------------------------------------

void xExtentsVisitor::apply(osg::Geode &node)
{
    osg::BoundingBox bb;

    // update bounding box
    for (size_t i = 0; i < node.getNumDrawables(); ++i)
        // expand overall bounding box
        bb.expandBy(node.getDrawable(i)->getBound());

    osg::BoundingBox xbb;

    // transform corners by current matrix
    for (size_t i = 0; i < 8; ++i)
    {
        osg::Vec3 xv = bb.corner(i) * m_TransformMatrix;
        xbb.expandBy(xv);
    }

    // update overall bounding box size
    m_BoundingBox.expandBy(xbb);

    // continue traversing the graph
    traverse(node);
}

//------------------------------------------------------------------------

void xExtentsVisitor::apply(osg::Transform &node)      // handle geode drawable extents to expand the box
{
    osg::Matrix matrixRestore(m_TransformMatrix);
    node.computeLocalToWorldMatrix(m_TransformMatrix, this);

    // Continue traversing the graph.
    traverse(node);

    // Restore the previous accumulated transformation.
    m_TransformMatrix = matrixRestore;
}

const osg::BoundingBox &xExtentsVisitor::GetBound() const
{
    return m_BoundingBox;
}
