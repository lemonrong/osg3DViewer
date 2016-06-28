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

#ifndef _OSGVIEWER_XSCENEMODEL_H_
#define _OSGVIEWER_XSCENEMODEL_H_

#ifdef min
#undef min
#endif

#include <osg/MatrixTransform>
#include <osgFX/Scribe>

#include <QtCore/QObject>
#include <osgShadow/ShadowedScene>

#include <iostream>

class xSceneModel : public QObject
{
    Q_OBJECT

public:
    xSceneModel(QObject *parent = NULL);
    virtual ~xSceneModel();

    void setData(osg::Node *data, bool resetHome = true);
    void resetModel();

    // debug
    bool saveSceneData(const std::string &);

	void setShadowEnabled(bool val);
    void setHighlightScene(bool val);

	void setLightingEnabled(bool bLightingOn) {m_isLightScene=bLightingOn;}
	bool getLightingEnabled() const {return m_isLightScene;}

	void setTextureEnabled(bool bTextureOn) {m_isTextrueScene = bTextureOn;}
	bool getTextureEnabled() const {return m_isTextrueScene;}
	
    void setBackfaceEnabled(bool bBackface) {m_isBackface = bBackface;}
    bool getBackfaceEnabled() const {return m_isBackface;}

    // get the root node of the scene
    osg::Node *getScene(void);

    // get the terrain node if exist
    osg::Node *getObject(void);

    const osg::Vec3 & getSceneCenter() {return m_sceneCenter;}

signals:
	void sigLoadBegin(bool);
	void sigLoadFinished();

private:
    // functions
    void createScene();

    osg::Group *createSceneLight();

    double getNextVal (double curr);

    void reoderUnderlays();

private:
    // Type node (visible, ir3, ...)
    std::map<int, osg::Node *> m_mapTypeNodes;

    // Palette node (default, 0, 1, 2, ...)
    std::map<int, osg::Node *> m_mapPaletteNodes;
    std::map<int, bool> m_mapHidePaletteNodes;

    bool m_hideInstrumentNodes;
    bool m_hideObjectNodes;
    std::map<int, bool> m_mapHideTypeNodes;

	osg::ref_ptr<osg::Switch> m_switchRoot;

    osg::ref_ptr<osg::Group> m_sceneNode;

    osg::ref_ptr<osg::MatrixTransform> m_transformSpinScene;
    //osg::MatrixTransform* m_transformSpinObject;

    osg::ref_ptr<osg::Node> m_currentData;

    osg::LightSource *m_defautLightSource;

    bool m_isHighlightScene;
	bool m_isShadowScene;
	bool m_isTextrueScene;
	bool m_isLightScene;
	bool m_isBackface;

    osg::Vec3 m_baseLightPosition;

    osg::ref_ptr<osgFX::Scribe> m_currentHightlight;

    osg::Vec3 m_sceneCenter;

	// root node of the model
	//osg::ref_ptr<osg::Group> m_scene;
	osg::ref_ptr<osgShadow::ShadowedScene> m_rootShadowNode;
};

#endif // _OSGVIEWER_XSCENEMODEL_H_
