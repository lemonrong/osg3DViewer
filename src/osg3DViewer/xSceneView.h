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

#ifndef _OSGVIEWER_OSGWINDOW_H_
#define _OSGVIEWER_OSGWINDOW_H_
#pragma once


#include <QtCore/QTimer>

#include <osgViewer/CompositeViewer>
#include <osgGA/StateSetManipulator>
#include <osgQt/GraphicsWindowQt>
#include <osgShadow/ShadowedScene>

class xSceneView : public QWidget, public osgViewer::CompositeViewer
{
public:
	xSceneView(QWidget *parent = NULL);
	virtual ~xSceneView(void) {}

	void setRefreshPeriod(unsigned int period);
	void setIdle(bool val);
	void setSceneData(osg::Node *);

	void setLightingEnabled(bool bLightingOn) { m_pStatesetManipulator->setLightingEnabled(bLightingOn);}
	bool getLightingEnabled() const {return m_pStatesetManipulator->getLightingEnabled(); }

	void setTextureEnabled(bool bTextureOn) {m_pStatesetManipulator->setTextureEnabled(bTextureOn); }
	bool getTextureEnabled() const {return m_pStatesetManipulator->getTextureEnabled(); }

	void setShadowEnabled(bool val);
	void home();

protected:
	QWidget* addViewWidget(osgQt::GraphicsWindowQt* gw, osg::Node* scene);
	osgQt::GraphicsWindowQt* createGraphicsWindow(int x, int y, int w, int h, const std::string& name="", bool windowDecoration=false);
	virtual void paintEvent( QPaintEvent* /* event */ )  { frame(); }

private:
	void createSceneEnvironnement();

private:
	QTimer m_timer;
	unsigned int m_refreshPeriod;
	osg::ref_ptr<osg::Camera> m_camera;
	osg::ref_ptr<osgViewer::View> m_view;
	osg::ref_ptr<osgGA::StateSetManipulator> m_pStatesetManipulator;

	// root node of the model
	osg::ref_ptr<osg::Group> m_scene;
	osg::ref_ptr<osgShadow::ShadowedScene> m_rootNodes;
};

#endif 
