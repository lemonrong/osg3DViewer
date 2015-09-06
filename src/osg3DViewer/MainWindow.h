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

#ifndef _OSGVIEWER_MAINWINDOW_H_
#define _OSGVIEWER_MAINWINDOW_H_

#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QLabel>
#include <QtCore/QStringList>
#include <QtCore/QString>
#include <QtCore/QModelIndex>
#include <osg/Node>

class xSceneView;
class xSceneModel;
class xTreeModel;
class xTreeView;

#include "ui_MainWindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool loadFile(const QString &file);

public slots:

    void on_actionQuit_triggered();
    void on_actionOpen_triggered();
	void on_actionUnload_triggered();
	void on_actionSave_As_triggered();
    void recentFileActivated(QAction *action);
    void setupRecentFilesMenu();

	void on_actionReset_View_triggered();
	void on_actionFull_Screen_triggered(bool on);
	void on_actionTexture_triggered(bool val);
	void on_actionLight_triggered(bool val);
	void on_actionHighLight_triggered(bool val);
	void on_actionShadow_triggered(bool val);

protected slots:

    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void closeEvent(QCloseEvent *event);

signals:

    void newFileToLoad(const QString &);

private:
	void saveSettings();
	void loadSettings();
    void addRecentlyOpenedFile(const QString &fn, QStringList &lst);
    void updateUi();
    void enableActions(bool);
    void updateApplicationIdentity();

	void hideDockWidgets();
	void showDockWidgets();

private:
	Ui::MainWindow ui;
	osg::ref_ptr<osg::Node> m_rootNode;
	xSceneView *m_pSceneView;
	xSceneModel *m_pSceneModel;

	QList<QWidget *> m_listDock;
	xTreeModel *m_pTreeModel;
	xTreeView *m_pTreeView;

    // variables
	QString m_currFile;
    QString m_lastDirectory;

    // recent files
    QMenu *m_recentFilesMenu;
    QStringList m_recentFiles;

   
    // display aspect ratio of the central windows
    QLabel *m_aspectRatioLabel;

};

#endif 
