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

#include <osgShadow/ShadowMap>
#include <osgShadow/SoftShadowMap>
#include <osgShadow/ParallelSplitShadowMap>
#include <osgShadow/LightSpacePerspectiveShadowMap>
#include <osgShadow/StandardShadowMap>

const unsigned int defaultRefreshPeriod = 30;
const unsigned int idleRefreshPeriod = 60;

const int receivesShadowTraversalMask = 0x1;
const int castsShadowTraversalMask = 0x2;

//------------------------------------------------------------------------------------------------------------

xSceneView::xSceneView(QWidget *parent) : QWidget(parent), m_refreshPeriod(defaultRefreshPeriod)
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

	createSceneEnvironnement();

	// by default all stateset are enabled
	//setBackfaceEnabled(true);
	//setLightingEnabled(true);
	//setTextureEnabled(true);
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
	m_pStatesetManipulator = new osgGA::StateSetManipulator(m_camera->getOrCreateStateSet());
	m_view->addEventHandler(m_pStatesetManipulator);
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

void xSceneView::createSceneEnvironnement()
{
	m_scene = new osg::Group(); // root node
	m_scene->setName("sceneGroup");

	// Create a clear Node

	//m_clearColor = osg::Vec4f(0.33f,0.33f,0.33f,1.0f);
	//m_backdrop = new osg::ClearNode;
	//m_backdrop->setName("backdrop");
	//m_backdrop->setClearColor(m_clearColor);
	//m_scene->addChild(m_backdrop);

	//// create and attach the gradient background
	//m_gradientBackground = createBackground();
	//m_scene->addChild(m_gradientBackground);

	// Attach the switch containing the root nodes
	m_rootNodes = new osgShadow::ShadowedScene();
	m_rootNodes->setReceivesShadowTraversalMask(receivesShadowTraversalMask);
	m_rootNodes->setCastsShadowTraversalMask(castsShadowTraversalMask);
	m_rootNodes->setDataVariance( osg::Object::DYNAMIC );

	m_scene->addChild(m_rootNodes);

	//m_grid = makeGrid();
	//m_scene->addChild(m_grid);
	//m_axis = makeAxis();
	//m_scene->addChild(m_axis);

	//// hide by default
	//m_grid->setNodeMask(0x0);
	//m_axis->setNodeMask(0x0);

	// for compass
	//m_cameraCompass = createCompass();
	//m_scene->addChild(m_cameraCompass);

	//createPivotManipulator();

	m_view->setSceneData(m_scene);
}

void xSceneView::setSceneData(osg::Node *node)
{
	//m_view->setSceneData(node);
	m_rootNodes->removeChild(0, m_rootNodes->getNumChildren());
	m_rootNodes->addChild(node);

	m_view->getCameraManipulator()->setAutoComputeHomePosition(false);

	m_rootNodes->dirtyBound();

	// save the current bbox;
	//ExtentsVisitor ext;
	//node->accept(ext);

	osg::BoundingSphere bound = m_rootNodes->getBound();

	m_view->getCameraManipulator()->setHomePosition( bound.center() + osg::Vec3( 1.5f * bound.radius(),1.5f * bound.radius(),1.5f * bound.radius()),
		bound.center(),	osg::Vec3(0.0f,0.0f,1.0f));
	m_view->home();
}

//void xSceneView::takeSnapshot()
//{}

void xSceneView::setShadowEnabled(bool val)
{
	
	if (val)
	{
		int alg = 3;
		const osg::BoundingSphere& bs = m_rootNodes->getBound();

		// test bidon pour modifier l'algo d'ombrage en fonction de la taille de l'objet
		if (alg == 0)
		{
			osg::ref_ptr<osgShadow::ParallelSplitShadowMap> pssm = new osgShadow::ParallelSplitShadowMap(NULL,5);
			pssm->setTextureResolution(2048);
			m_rootNodes->setShadowTechnique(pssm.get());

			// blindage mais je ne sais pas trop pourquoi ?!?
			pssm->init();
		}
		else if (alg == 2)
		{
			osg::ref_ptr<osgShadow::SoftShadowMap> pssm = new osgShadow::SoftShadowMap();

			pssm->setTextureSize( osg::Vec2s(2048,2048) );

			//pssm->setTextureResolution(2048);
			m_rootNodes->setShadowTechnique(pssm.get());

			// blindage mais je ne sais pas trop pourquoi ?!?
			pssm->init();
		}
		else if (alg == 3)
		{
			osg::ref_ptr<osgShadow::MinimalShadowMap> pssm = new osgShadow::LightSpacePerspectiveShadowMapDB();

			pssm->setTextureSize(osg::Vec2s(2048,2048));

			//pssm->setTextureResolution(2048);
			m_rootNodes->setShadowTechnique(pssm.get());

			// blindage mais je ne sais pas trop pourquoi ?!?
			pssm->init();
		}
	}
	else
	{
		m_rootNodes->setShadowTechnique(NULL);
	}
}

void xSceneView::home()
{
	//osg::Vec3d eye;
	//osg::Vec3d center;
	//osg::Vec3d up;

	// reset the pivot center point
	//m_view->getCameraManipulator()->getHomePosition(eye, center, up);
	//recenterPivotPoint( center.x(),center.y(),center.z() );

	m_view->home();
}
