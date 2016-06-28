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
#include "xExtentsVisitor.h"
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
	
	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
	m_timer->start(m_refreshPeriod);

	m_pickHandler = new xPickingHandler();
	m_view->setCameraManipulator(m_pickHandler.get());
	//createSceneEnvironnement();

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
	m_statesetManipulator = new osgGA::StateSetManipulator(m_camera->getOrCreateStateSet());
	m_view->addEventHandler(m_statesetManipulator);
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
	m_model = model;
	slotUpdateModel();
	
	connect(m_model, SIGNAL(sigLoadBegin(bool)), this, SLOT(slotResetView(bool)));
	connect(m_model, SIGNAL(sigLoadFinished()), this, SLOT(slotResetHome()));

	if (NULL != m_pickHandler)
	{
		connect(m_pickHandler.get(), SIGNAL(sigPicked(osg::Drawable*)),this, SIGNAL(sigPicked(osg::Drawable*)));
		//connect(m_pPickHandler.get(), SIGNAL(recenterViewTo(double,double,double)),this, SLOT(recenterPivotPoint(double,double,double)));

		//connect(m_pPickHandler.get(), SIGNAL(zoomViewIn()),this, SLOT(showZoomIn()));
		//connect(m_pPickHandler.get(), SIGNAL(zoomViewOut()),this, SLOT(showZoomOut()));
		//connect(m_pPickHandler.get(), SIGNAL(dragView()),this, SLOT(showDrag()));
		//connect(m_pPickHandler.get(), SIGNAL(rotateView()),this, SLOT(showPivot()));
	}
}

void xSceneView::setRefreshPeriod(unsigned int period)
{
	m_refreshPeriod = period;
}

void xSceneView::setIdle(bool val)
{
	if (m_timer == NULL)
		return;	
	if (val)
		m_timer->start(idleRefreshPeriod);
	else
		m_timer->start(defaultRefreshPeriod);
}

void xSceneView::slotResetView(bool reset)
{
	m_isResetHome = reset;
	if (!m_isResetHome)
	{
		// get the previous values;
		m_matrix = getTrackballManipulator()->getMatrix();
		m_center = getTrackballManipulator()->getCenter();
		m_distance = getTrackballManipulator()->getDistance();
	}
}

void xSceneView::slotResetHome()
{
	m_view->getCameraManipulator()->setAutoComputeHomePosition(false);

	osg::ref_ptr<osg::Node> ptrRootNode = m_model->getScene();
	ptrRootNode->dirtyBound();

	// save the current bbox;
	xExtentsVisitor ext;
	ptrRootNode->accept(ext);

	osg::BoundingSphere bound = ptrRootNode->getBound();

	m_view->getCameraManipulator()->setHomePosition(bound.center() + osg::Vec3(1.5f * bound.radius(),-3.0f * bound.radius(),1.5f * bound.radius()),
		bound.center(),
		osg::Vec3(0.0f,0.0f,1.0f));

	// compute the new scale of grid if enable
	//if (m_showGrid)
	//	setGridEnabled(true);

	if (m_isResetHome)
	{
		home();
	}
	else
	{
		// restore the view point
		getTrackballManipulator()->setDistance(m_distance);
		getTrackballManipulator()->setByMatrix(m_matrix);
		getTrackballManipulator()->setCenter(m_center);
	}
}
void xSceneView::slotPickGeometry(osg::Drawable *d)
{
	resetSelection();
}
void xSceneView::home()
{
	//osg::Vec3d eye;
	//osg::Vec3d center;
	//osg::Vec3d up;

	// reset the pivot center point
	//m_view->getCameraManipulator()->getHomePosition(eye, center, up);
	//recenterPivotPoint(center.x(),center.y(),center.z());

	m_view->home();
}

void xSceneView::setHighlightScene(bool val)
{
	if (m_model != NULL)
		m_model->setHighlightScene(val);
}

void xSceneView::setShadowEnabled(bool val)
{
	if (m_model != NULL)
		m_model->setShadowEnabled(val);
}

void xSceneView::setLightingEnabled(bool bLightingOn)
{
	if (m_statesetManipulator.valid())	
		m_statesetManipulator->setLightingEnabled(bLightingOn);
	if (m_model != NULL)
		m_model->setLightingEnabled(bLightingOn);
	
}
bool xSceneView::getLightingEnabled() const
{
	if (m_model != NULL)
		return m_model->getLightingEnabled();
	return false;
}

void xSceneView::setTextureEnabled(bool bTextureOn)
{
	if (m_statesetManipulator.valid())	
		m_statesetManipulator->setTextureEnabled(bTextureOn);
	if (m_model != NULL)
		m_model->setTextureEnabled(bTextureOn);
}
bool xSceneView::getTextureEnabled() const
{
	if (m_model != NULL)
		return m_model->getTextureEnabled();
	return false;
}
void xSceneView::setBackfaceEnabled(bool bBackface)
{
    if (m_statesetManipulator.valid())
        m_statesetManipulator->setBackfaceEnabled(bBackface);
    if (m_model != NULL)
        m_model->setBackfaceEnabled(bBackface);
}
bool xSceneView::getBackfaceEnabled() const
{
    if (m_model != NULL)
        return m_model->getBackfaceEnabled();
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
	if (m_model == NULL)
		return;

	osg::ref_ptr<osg::Node> ptrRoot = m_model->getScene();

	m_view->setSceneData(ptrRoot);
	if (m_statesetManipulator.valid())
	{
		m_statesetManipulator->setLightingEnabled(m_model->getLightingEnabled());
		m_statesetManipulator->setTextureEnabled(m_model->getTextureEnabled());
	}

	if (ptrRoot.valid())
	{
		osg::BoundingSphere bound = ptrRoot->getBound();
		m_view->getCameraManipulator()->setHomePosition(bound.center() + osg::Vec3(1.5f * bound.radius(),1.5f * bound.radius(),1.5f * bound.radius()),
			bound.center(),	osg::Vec3(0.0f,0.0f,1.0f));
		m_view->home();
	}
}
bool xSceneView::centerOnNode(osg::Node* node)
{
	if (!node)
		return false;

	const osg::BoundingSphere& bs = node->getBound();
	if (bs.radius() < 0.0) // invalid node
		return false;

	// world matrix transform
	osg::Matrix mat = matrixListtoSingle(node->getWorldMatrices());
	m_pickHandler->setCenter(bs.center() * mat);
	m_pickHandler->setDistance(3.0 * bs.radius());

	return true;
}
osg::Matrix xSceneView::matrixListtoSingle(const osg::MatrixList &tmplist)
{
	osg::Matrix tmp;

	if (tmplist.size() > 0)
	{
		unsigned int i;
		for (i = 1, tmp = tmplist[0]; i < tmplist.size(); i++)
			tmp *= tmplist[0];
		tmp = tmplist[0];
	}
	return (tmp);
}
void xSceneView::resizeEvent(QResizeEvent * event)
{
	//if (m_statsHandler)
	//	m_statsHandler->setWindowSize(width(), height());

	//if (m_cameraCompass)
	//{
	//	m_cameraCompass->setProjectionMatrix(osg::Matrix::ortho2D(0.0,width(),0.0,height()));
	//	osg::MatrixTransform *mat = dynamic_cast<osg::MatrixTransform *>(m_cameraCompass->getChild(0));
	//	mat->setMatrix(osg::Matrix::translate(width() - compassSize / 2.0f - compassOffset,compassSize / 2.0f + compassOffset,0.0f));
	//}

	// inform that aspect rastio has changed
	emit sigNewAspectRatio(QSize(width(), height()));
	QWidget::resizeEvent(event);
}