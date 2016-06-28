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
    m_isHighlightScene(false),
	m_isShadowScene(false),
	m_isTextrueScene(false),
	m_isLightScene(false),
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
    return osgDB::writeNodeFile(*m_currentData, file);
}

void xSceneModel::setHighlightScene(bool val)
{
	if (m_isHighlightScene == val)
		return;

	m_isHighlightScene = val;
	if (m_isHighlightScene)
	{
		//osg::Group *parent = m_rootNodes->getParent(0);
		m_currentHightlight = new osgFX::Scribe();
		m_currentHightlight->setWireframeLineWidth(2.0);
		m_currentHightlight->setWireframeColor(osg::Vec4(1.0,1.0,1.0,1.0));

		m_currentHightlight->addChild(m_rootShadowNode);
		m_sceneNode->replaceChild(m_rootShadowNode, m_currentHightlight);
	}
	else
	{
		//osgFX::Scribe* parentAsScribe = m_currentHightlight;
		//osg::Node::ParentList parentList = parentAsScribe->getParents();
		//for(osg::Node::ParentList::iterator itr = parentList.begin();
		//	itr!=parentList.end();
		//	++itr)
		//	(*itr)->replaceChild(parentAsScribe,parentAsScribe->getChild(0));
		m_sceneNode->replaceChild(m_currentHightlight, m_rootShadowNode);
	}
}

osg::Node *xSceneModel::getScene()
{
    return m_switchRoot;
}

osg::Node *xSceneModel::getObject()
{
    return m_currentData;
}

void xSceneModel::createScene()
{
    // init
    m_switchRoot = new osg::Switch(); // switch
    m_switchRoot->setName("rootSwitch");

    m_switchRoot->addChild(createSceneLight());
    // create and attach the scene nodes
    m_transformSpinScene = new osg::MatrixTransform; // spin transform for global scene
    m_transformSpinScene->setName("spinTransformScene");
    m_switchRoot->addChild(m_transformSpinScene);

    m_sceneNode = new osg::Group(); // scene
    m_sceneNode->setName("NodeScene");
    m_transformSpinScene->addChild(m_sceneNode);

	// shadow node
	m_rootShadowNode = new osgShadow::ShadowedScene();
	m_rootShadowNode->setReceivesShadowTraversalMask(receivesShadowTraversalMask);
	m_rootShadowNode->setCastsShadowTraversalMask(castsShadowTraversalMask);
	m_rootShadowNode->setDataVariance(osg::Object::DYNAMIC);
	m_sceneNode->addChild(m_rootShadowNode.get());

	m_currentData = new osg::Node();
	m_currentData->setName("currentData");
	m_rootShadowNode->addChild(m_currentData.get());

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

    m_rootShadowNode->removeChild(m_currentData.get());
    m_currentData = data;

    if (!data)
        return;

    m_currentData->setName("currentData");

    m_rootShadowNode->addChild(m_currentData.get());

    // translate all the underlay layers to bottom
    //ExtentsVisitor ext;
    //m_currentData->accept(ext);

    m_sceneCenter = m_sceneNode->getBound().center();

    if (m_isHighlightScene)
    {
        setHighlightScene(false);
        setHighlightScene(true);
    }

	setShadowEnabled(m_isShadowScene);
    emit sigLoadFinished();
}

void xSceneModel::setShadowEnabled(bool val)
{
	if (val)
	{
		int alg = 3;
		const osg::BoundingSphere& bs = m_rootShadowNode->getBound();
		// test bidon pour modifier l'algo d'ombrage en fonction de la taille de l'objet
		if (alg == 0)
		{
			osg::ref_ptr<osgShadow::ParallelSplitShadowMap> pssm = new osgShadow::ParallelSplitShadowMap(NULL,5);
			pssm->setTextureResolution(2048);
			m_rootShadowNode->setShadowTechnique(pssm.get());

			pssm->init();
		}
		else if (alg == 2)
		{
			osg::ref_ptr<osgShadow::SoftShadowMap> pssm = new osgShadow::SoftShadowMap();

			pssm->setTextureSize(osg::Vec2s(2048,2048));

			//pssm->setTextureResolution(2048);
			m_rootShadowNode->setShadowTechnique(pssm.get());

			pssm->init();
		}
		else if (alg == 3)
		{
			osg::ref_ptr<osgShadow::MinimalShadowMap> pssm = new osgShadow::LightSpacePerspectiveShadowMapDB();
			pssm->setTextureSize(osg::Vec2s(2048,2048));

			//pssm->setTextureResolution(2048);
			m_rootShadowNode->setShadowTechnique(pssm.get());

			pssm->init();
		}
	}
	else
	{
		m_rootShadowNode->setShadowTechnique(NULL);
	}
}
