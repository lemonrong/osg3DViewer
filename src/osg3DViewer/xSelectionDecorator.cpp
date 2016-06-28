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

#include "xSelectionDecorator.h"

#include <osgDB/Registry>

#include <osgDB/FileUtils>

/*!
    Constructor
 */

xSelectionDecorator::xSelectionDecorator() : m_bEnable(false)
{}

/*!
    Destructor
 */
xSelectionDecorator::~xSelectionDecorator()
{
}

/*!
    Enable or disable selection highlighting.
    @param[in] enable Enable state.
 */
void xSelectionDecorator::setEnable(bool enable)
{
    m_bEnable = enable;

    if (m_bEnable)
        setNodeMask(-1);
    else
        setNodeMask(0);
}

/*!
    Return 'true' if selection highlighting is enabled.
    @return Enable state.
 */
bool xSelectionDecorator::getEnable() const
{
    return m_bEnable;
}

void xSelectionDecorator::traverse(osg::NodeVisitor& nv)
{
    osg::Group::traverse(nv);
}

/*!
    Constructor
 */
xDefaultSelectionDecorator::xDefaultSelectionDecorator() : xSelectionDecorator()
{
    // create highlighting state
    m_pStateSet = new osg::StateSet;
    m_polygonOffset = new osg::PolygonOffset;
    m_polygonOffset->setFactor(-1.0f);
    m_polygonOffset->setUnits(-1.0f);
    m_polygonMode = new osg::PolygonMode;
    m_polygonMode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
    m_pStateSet->setAttributeAndModes(m_polygonOffset.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    m_pStateSet->setAttributeAndModes(m_polygonMode.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    m_lineWidth = new osg::LineWidth;
    m_lineWidth->setWidth(4.0f);
    m_pStateSet->setAttributeAndModes(m_lineWidth.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    m_material = new osg::Material;
    m_material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(1.0, 0.0, 0.0, 1.0));
    m_material->setColorMode(osg::Material::DIFFUSE);
    m_pStateSet->setAttributeAndModes(m_material.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    m_pStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);
    m_pStateSet->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);
    setStateSet(m_pStateSet.get());

    setEnable(true);
}

/*!
    Destructor
 */
xDefaultSelectionDecorator::~xDefaultSelectionDecorator()
{
}

/*!
    Set offset factor of polygon offset attribute.
    @param[in] factor Offset factor of polygon offset attribute.
 */
void xDefaultSelectionDecorator::setOffsetFactor(float factor)
{
    m_polygonOffset->setFactor(factor);
}

/*!
    Return offset factor of polygon offset attribute.
    @return Offset factor of polygon offset attribute.
 */
float xDefaultSelectionDecorator::getOffsetFactor() const
{
    return m_polygonOffset->getFactor();
}

/*!
    Set offset units of polygon offset attribute.
    @param[in] units Offset units of polygon offset attribute.
 */
void xDefaultSelectionDecorator::setOffsetUnits(float units)
{
    m_polygonOffset->setUnits(units);
}

/*!
    Return offset units of polygon offset attribute.
    @return Offset units of polygon offset attribute.
 */
float xDefaultSelectionDecorator::getOffsetUnits() const
{
    return m_polygonOffset->getUnits();
}

/*!
    Set polygon mode attribute for highlight rendering.
    @param[in] mode Polygon mode.
 */
void xDefaultSelectionDecorator::setPolygonMode(osg::PolygonMode::Mode mode)
{
    m_polygonMode->setMode(osg::PolygonMode::FRONT_AND_BACK, mode);
}

/*!
    Return polygon mode attribute for highlight rendering.
    @return Polygon mode.
 */
osg::PolygonMode::Mode xDefaultSelectionDecorator::getPolygonMode() const
{
    return m_polygonMode->getMode(osg::PolygonMode::FRONT_AND_BACK);
}

/*!
    Set line width attribute for highlight rendering.
    @param[in] width Line width.
 */
void xDefaultSelectionDecorator::setLineWidth(float width)
{
    m_lineWidth->setWidth(width);
}

/*!
    Return line width attribute for highlight rendering.
    @return Line width.
 */
float xDefaultSelectionDecorator::getLineWidth() const
{
    return m_lineWidth->getWidth();
}

/*!
    Set highlight color.
    @param[in] color Highlight color.
 */
void xDefaultSelectionDecorator::setEmissionColor(osg::Vec4 color)
{
    m_material->setEmission(osg::Material::FRONT_AND_BACK, color);
}

/*!
    Return highlight color.
    @return Highlight color.
 */
osg::Vec4 xDefaultSelectionDecorator::getEmissionColor() const
{
    return m_material->getEmission(osg::Material::FRONT_AND_BACK);
}

/*!
    Enable or disable textureing for highlight rendering.
    @param[in] texturing Texturing state.
 */
void xDefaultSelectionDecorator::setTexturing(bool texturing)
{
    m_pStateSet->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::OVERRIDE | (texturing ? (osg::StateAttribute::ON): (osg::StateAttribute::OFF)));
}

/*!
    Return textureing state for highlight rendering.
    @return Texturing state.
 */
bool xDefaultSelectionDecorator::getTexturing() const
{
    return m_pStateSet->getTextureMode(0, GL_TEXTURE_2D) & osg::StateAttribute::ON;
}

/*!
    Enable or disable lighting for highlight rendering.
    @param[in] lighting Lighting state.
 */
void xDefaultSelectionDecorator::setLighting(bool lighting)
{
    m_pStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | (lighting ? (osg::StateAttribute::ON): (osg::StateAttribute::OFF)));
}

/*!
    Return lighting state for highlight rendering.
    @return Lighting state.
 */
bool xDefaultSelectionDecorator::getLighting() const
{
    return m_pStateSet->getMode(GL_LIGHTING) & osg::StateAttribute::ON;
}
