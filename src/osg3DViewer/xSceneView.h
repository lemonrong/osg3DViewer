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

#include "xPickingHandler.h"
//#include <osgShadow/ShadowedScene>

//#include <osgFX/Scribe>
class xSceneModel;
class xSelectionManager;
class xSceneView : public QWidget, public osgViewer::CompositeViewer
{
	Q_OBJECT
public:
	xSceneView(QWidget *parent = NULL);
	virtual ~xSceneView(void) {}

	void setModel(xSceneModel *pModel);
	void setRefreshPeriod(unsigned int period);
	void setIdle(bool val);
	//void setSceneData(osg::Node *);

	void setLightingEnabled(bool bLightingOn);
	bool getLightingEnabled() const;

	void setTextureEnabled(bool bTextureOn);
	bool getTextureEnabled() const;
	
    void setBackfaceEnabled(bool bBackface);
    bool getBackfaceEnabled() const;

	void setHighlightScene(bool val);

	void setShadowEnabled(bool val);
	void home();

	void resetSelection();

	bool highlight(osg::Node* node);
	bool centerOnNode(osg::Node* node);

	void resizeEvent(QResizeEvent * event);

	osgGA::TrackballManipulator *getTrackballManipulator(){return m_pPickHandler.get();}

public slots:
	void slotUpdateModel();

	void slotResetHome();
	void slotResetView(bool reset = true);
	void slotPickGeometry(osg::Drawable *d);

signals:
	//void newScreenshotAvailable(osg::Image *);
	void sigNewAspectRatio(const QSize &);
	void sigPicked(osg::Drawable *);

protected:
	QWidget* addViewWidget(osgQt::GraphicsWindowQt* gw, osg::Node* scene);
	osgQt::GraphicsWindowQt* createGraphicsWindow(int x, int y, int w, int h, const std::string& name="", bool windowDecoration=false);
	virtual void paintEvent(QPaintEvent* /* event */)  {frame();}

private:
	 osg::Matrix matrixListtoSingle(const osg::MatrixList &tmplist);

private:
	QTimer m_timer;
	unsigned int m_refreshPeriod;
	osg::ref_ptr<osg::Camera> m_pCamera;
	osg::ref_ptr<osgViewer::View> m_pView;
	osg::ref_ptr<osgGA::StateSetManipulator> m_pStatesetManipulator;
	osg::ref_ptr<xPickingHandler>  m_pPickHandler;

	bool m_bResetHome;
	osg::Matrixd m_matrix;
	osg::Vec3d m_vCenter;
	double m_dDistance;

	xSceneModel *m_pModel;
	xSelectionManager *m_pSelectionManager;
};

#endif 
