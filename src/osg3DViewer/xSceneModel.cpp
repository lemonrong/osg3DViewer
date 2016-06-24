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
#include "xOsgLogger.h"
#include "xLogHandler.h"
//#include "ExtentsVisitor.h"

#include <osg/PolygonOffset>
#include <osg/ShapeDrawable>
#include <osgShadow/ParallelSplitShadowMap>
#include <osgShadow/ShadowMap>
#include <osgShadow/SoftShadowMap>
#include <osgDB/WriteFile>
#include <osg/CullFace>

#include <osgManipulator/CommandManager>
#include <osgManipulator/TabBoxDragger>
#include <osgManipulator/TrackballDragger>

#include <osgShadow/ShadowMap>
#include <osgShadow/SoftShadowMap>
#include <osgShadow/ParallelSplitShadowMap>
#include <osgShadow/LightSpacePerspectiveShadowMap>
#include <osgShadow/StandardShadowMap>

#include <osgUtil/Statistics>

// typedefs and definitions
#if   !defined(CLAMP)
#define  CLAMP(x,min,max)   ((x<min) ? min : ((x>max) ? max : x))
#endif

const int receivesShadowTraversalMask = 0x1;
const int castsShadowTraversalMask = 0x2;

// --------------------------------------------------------------------------------
xSceneModel::xSceneModel(QObject *parent) :
    QObject(parent),
    m_bHighlightScene(false),
	m_bShadowScene(false),
	m_bTextrueScene(false),
	m_bLightScene(false),
    m_sceneCenter(osg::Vec3(0.0f,0.0f,0.0f))
{
    // set the osg log to QT message
    osg::setNotifyLevel(osg::NOTICE);
    xOsgLogger *loggerCout = new xOsgLogger(std::cout);
    connect(loggerCout, SIGNAL(sigMessage(const QString &)), xLogHandler::getInstance(), SLOT(slotReportDebug(const QString &)),Qt::QueuedConnection);
    xOsgLogger *loggerCerr = new xOsgLogger(std::cerr);
    connect(loggerCerr, SIGNAL(sigMessage(const QString &)),  xLogHandler::getInstance(), SLOT(slotReportInfo(const QString &)),Qt::QueuedConnection);

    //osg::setNotifyLevel(osg::DEBUG_INFO);
    //osg::setNotifyLevel(osg::NOTICE);

    createScene();
}

xSceneModel::~xSceneModel()
{}

void xSceneModel::resetModel()
{
    setData(new osg::Node());
}

bool xSceneModel::saveSceneData(const std::string & file)
{
    return osgDB::writeNodeFile(*m_pCurrentData, file);
}

void xSceneModel::setHighlightScene(bool val)
{
	if (m_bHighlightScene == val)
		return;

	m_bHighlightScene = val;
	if (m_bHighlightScene)
	{
		//osg::Group *parent = m_rootNodes->getParent(0);
		m_pCurrentHightlight = new osgFX::Scribe();
		m_pCurrentHightlight->setWireframeLineWidth(2.0);
		m_pCurrentHightlight->setWireframeColor(osg::Vec4(1.0,1.0,1.0,1.0));

		m_pCurrentHightlight->addChild(m_ptrRootShadowNodes);
		m_pNodeScene->replaceChild(m_ptrRootShadowNodes, m_pCurrentHightlight);
	}
	else
	{
		//osgFX::Scribe* parentAsScribe = m_currentHightlight;
		//osg::Node::ParentList parentList = parentAsScribe->getParents();
		//for(osg::Node::ParentList::iterator itr = parentList.begin();
		//	itr!=parentList.end();
		//	++itr)
		//	(*itr)->replaceChild(parentAsScribe,parentAsScribe->getChild(0));
		m_pNodeScene->replaceChild(m_pCurrentHightlight, m_ptrRootShadowNodes);
	}
}

osg::Node *xSceneModel::getScene()
{
    return m_pSwitchRoot;
}

osg::Node *xSceneModel::getObject()
{
    return m_pCurrentData;
}

void xSceneModel::createScene()
{
    // init
    m_pSwitchRoot = new osg::Switch(); // switch
    m_pSwitchRoot->setName("rootSwitch");

    m_pSwitchRoot->addChild(createSceneLight());

    // create and attach the scene nodes
    m_pTransformSpinScene = new osg::MatrixTransform; // spin transform for global scene
    m_pTransformSpinScene->setName("spinTransformScene");
    m_pSwitchRoot->addChild(m_pTransformSpinScene);

    m_pNodeScene = new osg::Group(); // scene
    m_pNodeScene->setName("NodeScene");
    m_pTransformSpinScene->addChild(m_pNodeScene);

	// shadow node
	m_ptrRootShadowNodes = new osgShadow::ShadowedScene();
	m_ptrRootShadowNodes->setReceivesShadowTraversalMask(receivesShadowTraversalMask);
	m_ptrRootShadowNodes->setCastsShadowTraversalMask(castsShadowTraversalMask);
	m_ptrRootShadowNodes->setDataVariance(osg::Object::DYNAMIC);

	m_pNodeScene->addChild(m_ptrRootShadowNodes.get());

	m_pCurrentData = new osg::Node();
	m_pCurrentData->setName("currentData");
	m_ptrRootShadowNodes->addChild(m_pCurrentData.get());

}

osg::Group * xSceneModel::createSceneLight()
{
    osg::Group *grp = new osg::Group;

    // create and attach the light source
    osg::LightSource *lightSource = new osg::LightSource;
    lightSource->setLocalStateSetModes(osg::StateAttribute::ON);

    lightSource->getLight()->setAmbient(osg::Vec4(1.0,1.0,1.0,1.0));
    lightSource->getLight()->setDiffuse(osg::Vec4(1.0,1.0,1.0,1.0));
    lightSource->getLight()->setPosition(osg::Vec4(0.f,1000.0f,1000.0f,0.0f));
    grp->addChild(lightSource);

    return grp;
}

void xSceneModel::setData(osg::Node *data, bool resetHome)
{
    osg::Matrixd matrix;
    osg::Vec3d center;

    // reset selection
    emit sigLoadBegin(resetHome);

    m_sceneCenter = osg::Vec3(0.0f,0.0f,0.0f);

    m_ptrRootShadowNodes->removeChild(m_pCurrentData.get());
    m_pCurrentData = data;

    if (!data)
        return;

    m_pCurrentData->setName("currentData");

    m_ptrRootShadowNodes->addChild(m_pCurrentData.get());

    // translate all the underlay layers to bottom
    //ExtentsVisitor ext;
    //m_currentData->accept(ext);

    m_sceneCenter = m_pNodeScene->getBound().center();

    if (m_bHighlightScene)
    {
        setHighlightScene(false);
        setHighlightScene(true);
    }

	setShadowEnabled(m_bShadowScene);
    emit sigLoadFinished();
}

void xSceneModel::setShadowEnabled(bool val)
{
	if (val)
	{
		int alg = 3;
		const osg::BoundingSphere& bs = m_ptrRootShadowNodes->getBound();
		// test bidon pour modifier l'algo d'ombrage en fonction de la taille de l'objet
		if (alg == 0)
		{
			osg::ref_ptr<osgShadow::ParallelSplitShadowMap> pssm = new osgShadow::ParallelSplitShadowMap(NULL,5);
			pssm->setTextureResolution(2048);
			m_ptrRootShadowNodes->setShadowTechnique(pssm.get());

			pssm->init();
		}
		else if (alg == 2)
		{
			osg::ref_ptr<osgShadow::SoftShadowMap> pssm = new osgShadow::SoftShadowMap();

			pssm->setTextureSize(osg::Vec2s(2048,2048));

			//pssm->setTextureResolution(2048);
			m_ptrRootShadowNodes->setShadowTechnique(pssm.get());

			pssm->init();
		}
		else if (alg == 3)
		{
			osg::ref_ptr<osgShadow::MinimalShadowMap> pssm = new osgShadow::LightSpacePerspectiveShadowMapDB();
			pssm->setTextureSize(osg::Vec2s(2048,2048));

			//pssm->setTextureResolution(2048);
			m_ptrRootShadowNodes->setShadowTechnique(pssm.get());

			pssm->init();
		}
	}
	else
	{
		m_ptrRootShadowNodes->setShadowTechnique(NULL);
	}
}
