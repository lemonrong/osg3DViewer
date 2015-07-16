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

#include "xSceneView.h"
#include <QtGui/QGridLayout>

#include <osgDB/WriteFile>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/MultiTouchTrackballManipulator>

const unsigned int defaultRefreshPeriod = 30;
const unsigned int idleRefreshPeriod = 60;

//------------------------------------------------------------------------------------------------------------

xSceneView::xSceneView(QWidget *parent) : QWidget(parent), m_refreshPeriod(defaultRefreshPeriod)
{
	setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);	

	QWidget* widget = addViewWidget(createGraphicsWindow(0,0,100,100), NULL);

	QGridLayout* grid = new QGridLayout;

	// maximal area for widget
	grid->setContentsMargins(1,1,1,1);
	grid->setSpacing(0);
	grid->addWidget(widget, 0, 0);
	setLayout(grid);

	connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()));
	m_timer.start(m_refreshPeriod);
}

QWidget* xSceneView::addViewWidget(osgQt::GraphicsWindowQt* gw, osg::Node* scene)
{
	m_view = new osgViewer::View;
	addView(m_view.get());

	m_camera = m_view->getCamera();
	m_camera->setGraphicsContext(gw);
	
	const osg::GraphicsContext::Traits* traits = gw->getTraits();

	m_camera->setClearColor(osg::Vec4(0.2, 0.2, 0.6, 1.0));
	m_camera->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
	m_camera->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0f, 10000.0f);

	m_view->setSceneData(scene);
	m_view->addEventHandler(new osgViewer::StatsHandler);
	m_statesetManipulator = new osgGA::StateSetManipulator(m_camera->getOrCreateStateSet());
	m_view->addEventHandler(m_statesetManipulator);
	m_view->setCameraManipulator(new osgGA::MultiTouchTrackballManipulator);
	gw->setTouchEventsEnabled(true);
	return gw->getGLWidget();
}
osgQt::GraphicsWindowQt* xSceneView::createGraphicsWindow( int x, int y, int w, int h, const std::string& name, bool windowDecoration)
{
	osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	traits->windowName = name;
	traits->windowDecoration = windowDecoration;
	traits->x = x;
	traits->y = y;
	traits->width = w;
	traits->height = h;
	traits->doubleBuffer = true;
	traits->alpha = ds->getMinimumNumAlphaBits();
	traits->stencil = ds->getMinimumNumStencilBits();
	traits->sampleBuffers = ds->getMultiSamples();
	traits->samples = ds->getNumMultiSamples();

	return new osgQt::GraphicsWindowQt(traits.get());
}

void xSceneView::setRefreshPeriod(unsigned int period)
{
	m_refreshPeriod = period;
}

void xSceneView::setIdle(bool val)
{
	if (val)
		m_timer.start(idleRefreshPeriod);
	else
		m_timer.start(defaultRefreshPeriod);
}

void xSceneView::setSceneData(osg::Node *node)
{
	m_view->setSceneData(node);
}

//void xSceneView::takeSnapshot()
//{}



