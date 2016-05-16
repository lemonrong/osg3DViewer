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
 
#ifndef _OSG3DVIEWER_SHADERSELECTIONDECORATOR_H_
#define _OSG3DVIEWER_SHADERSELECTIONDECORATOR_H_

#include <osg/Group>
#include <osg/StateSet>
#include <osg/PolygonOffset>
#include <osg/PolygonMode>
#include <osg/LineWidth>
#include <osg/Material>

#include "xSelectionDecorator.h"

/*!
    @brief Selection decorator class for selection highlighting bu shader.
 */

class xShaderSelectionDecorator : public xSelectionDecorator
{
public:

    // Initialization
    xShaderSelectionDecorator();           //!< Constructor
    virtual ~xShaderSelectionDecorator();  //!< Destructor

    virtual void traverse(osg::NodeVisitor& nv);

protected:

private:

    bool loadShader();

    osg::ref_ptr<osg::PolygonOffset> m_pPolyOffset; //!< PolygonOffset attribute for highlighting.
    osg::ref_ptr<osg::Program> m_program;
    osg::ref_ptr<osg::Uniform> m_glowFactor;
};

#endif // _OSG3DVIEWER_SHADERSELECTIONDECORATOR_H_
