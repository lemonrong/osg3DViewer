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
#include "xSceneModel.h"
#include "xSceneView.h"
#include "xSelectionManager.h"
#include "xShaderSelectionDecorator.h"
#include <QtGui/QGridLayout>

#include <osgDB/WriteFile>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/MultiTouchTrackballManipulator>



const unsigned int defaultRefreshPeriod = 30;
const unsigned int idleRefreshPeriod = 60;

const int receivesShadowTraversalMask = 0x1;
const int castsShadowTraversalMask = 0x2;

//------------------------------------------------------------------------------------------------------------

xSceneView::xSceneView(QWidget *parent) : QWidget(parent), m_refreshPeriod(defaultRefreshPeriod), m_selectionManager(NULL)
{
	setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);
	setKeyEventSetsDone(0);

	QWidget* widget = addViewWidget(createGraphicsWindow(0,0,100,100), NULL);

	QGridLayout* grid = new QGridLayout;

	// maximal area for widget
	grid->setContentsMargins(1,1,1,1);
	grid->setSpacing(0);
	grid->addWidget(widget, 0, 0);
	setLayout(grid);

	connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()));
	m_timer.start(m_refreshPeriod);

	m_pPickHandler = new xPickingHandler();
	m_pView->setCameraManipulator(m_pPickHandler.get());
	//createSceneEnvironnement();

	// by default all stateset are enabled
	//setBackfaceEnabled(true);
	//setLightingEnabled(true);
	//setTextureEnabled(true);
}

QWidget* xSceneView::addViewWidget(osgQt::GraphicsWindowQt* gw, osg::Node* scene)
{
	m_pView = new osgViewer::View;
	addView(m_pView.get());

	m_pCamera = m_pView->getCamera();
	m_pCamera->setGraphicsContext(gw);
	
	const osg::GraphicsContext::Traits* traits = gw->getTraits();

	m_pCamera->setClearColor(osg::Vec4(0.2, 0.2, 0.6, 1.0));
	m_pCamera->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
	m_pCamera->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0f, 10000.0f);

	m_pView->setSceneData(scene);
	m_pView->addEventHandler(new osgViewer::StatsHandler);
	m_pStatesetManipulator = new osgGA::StateSetManipulator(m_pCamera->getOrCreateStateSet());
	m_pView->addEventHandler(m_pStatesetManipulator);
	//m_pView->setCameraManipulator(new osgGA::MultiTouchTrackballManipulator);
	gw->setTouchEventsEnabled(true);
	return gw->getGLWidget();
}
osgQt::GraphicsWindowQt* xSceneView::createGraphicsWindow(int x, int y, int w, int h, const std::string& name, bool windowDecoration)
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
void xSceneView::setModel(xSceneModel *model)
{
	m_pModel = model;
	slotUpdateModel();

	// clean previous elements
	////for (int i = m_rootNodes->getNumChildren() - 1; i >= 0; i--)
	////	m_rootNodes->removeChild(i);

	
	//connect(m_model, SIGNAL(loadBegin(bool)), this, SLOT(resetView(bool)));
	//connect(m_model, SIGNAL(loadFinished()), this, SLOT(resetHome()));

	if (NULL != m_pPickHandler)
	{
		connect(m_pPickHandler.get(), SIGNAL(sigPicked(osg::Drawable*)),this, SIGNAL(sigPicked(osg::Drawable*)));
		//connect(m_atm.get(), SIGNAL(recenterViewTo(double,double,double)),this, SLOT(recenterPivotPoint(double,double,double)));

		//connect(m_atm.get(), SIGNAL(zoomViewIn()),this, SLOT(showZoomIn()));
		//connect(m_atm.get(), SIGNAL(zoomViewOut()),this, SLOT(showZoomOut()));
		//connect(m_atm.get(), SIGNAL(dragView()),this, SLOT(showDrag()));
		//connect(m_atm.get(), SIGNAL(rotateView()),this, SLOT(showPivot()));
	}
	

	//m_rootNodes->addChild(m_model->getScene());
	
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


void xSceneView::home()
{
	//osg::Vec3d eye;
	//osg::Vec3d center;
	//osg::Vec3d up;

	// reset the pivot center point
	//m_view->getCameraManipulator()->getHomePosition(eye, center, up);
	//recenterPivotPoint(center.x(),center.y(),center.z());

	m_pView->home();
}

void xSceneView::setHighlightScene(bool val)
{
	if (m_pModel != NULL)
		m_pModel->setHighlightScene(val);
}

void xSceneView::setShadowEnabled(bool val)
{
	if (m_pModel != NULL)
		m_pModel->setShadowEnabled(val);
}

void xSceneView::setLightingEnabled(bool bLightingOn)
{
	if (m_pStatesetManipulator.valid())	
		m_pStatesetManipulator->setLightingEnabled(bLightingOn);
	if (m_pModel != NULL)
		m_pModel->setLightingEnabled(bLightingOn);
	
}
bool xSceneView::getLightingEnabled() const
{
	if (m_pModel != NULL)
		return m_pModel->getLightingEnabled();
	return false;
}

void xSceneView::setTextureEnabled(bool bTextureOn)
{
	if (m_pStatesetManipulator.valid())	
		m_pStatesetManipulator->setTextureEnabled(bTextureOn);
	if (m_pModel != NULL)
		m_pModel->setTextureEnabled(bTextureOn);
}
bool xSceneView::getTextureEnabled() const
{
	if (m_pModel != NULL)
		return m_pModel->getTextureEnabled();
	return false;
}
bool xSceneView::highlight(osg::Node* node)
{
	if (!m_selectionManager)
	{
		m_selectionManager = new xSelectionManager;
		m_selectionManager->setSelectionDecorator(new xShaderSelectionDecorator);
	}

	resetSelection();
	m_selectionManager->select(node);
	return true;
}
void xSceneView::resetSelection()
{
	if (m_selectionManager)
		m_selectionManager->clearSelection();
}

void xSceneView::slotUpdateModel()
{
	if (m_pModel == NULL)
		return;

	osg::ref_ptr<osg::Node> ptrRoot = m_pModel->getScene();

	m_pView->setSceneData(ptrRoot);
	if (m_pStatesetManipulator.valid())
	{
		m_pStatesetManipulator->setLightingEnabled(m_pModel->getLightingEnabled());
		m_pStatesetManipulator->setTextureEnabled(m_pModel->getTextureEnabled());
	}

	if (ptrRoot.valid())
	{
		osg::BoundingSphere bound = ptrRoot->getBound();
		m_pView->getCameraManipulator()->setHomePosition(bound.center() + osg::Vec3(1.5f * bound.radius(),1.5f * bound.radius(),1.5f * bound.radius()),
			bound.center(),	osg::Vec3(0.0f,0.0f,1.0f));
		m_pView->home();
	}
}
