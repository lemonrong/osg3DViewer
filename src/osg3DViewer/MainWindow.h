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
class xPropertyWidget;
class xObjectLoader;
class QCompleter;
class xSearchLineEdit;
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
	void on_actionOptimize_triggered(bool val);
	void on_actionBackFace_triggered(bool val);
	
	void on_actionLoad_Shader_triggered();
	void on_actionRemove_Shader_triggered();
	
    void on_actionEnableNode_triggered();
    void on_actionDisableNode_triggered();
    void on_actionExpandTree_triggered();
    void on_actionCollapseTree_triggered();
    void on_actionCenterOnSelection_triggered();
    void slotTreeViewCustomContextMenuRequested(const QPoint& pos);

	void on_pushButtonClearLog_pressed();
	void on_pushButtonSaveLog_pressed();

	// search
	void slotSearchCaseSensitiveToggled(bool checked);
	void slotEditSearchReturnPressed();

	void slotTreeNodeSelected(const QModelIndex &index);
	void slotSelectTreeItem(osg::Drawable *);
	void slotNewLoadedFile(osg::Node *);

	void slotPrintToLog(const QString & mess);
	void slotPrintToLog(const QStringList & mess);
	void slotChangeAspectRatio(const QSize & sz);
	
protected slots:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
    void keyPressEvent(QKeyEvent *event);

signals:
    void sigNewFileToLoad(const QString &);

private:
	void saveSettings();
	void loadSettings();
    void addRecentlyOpenedFile(const QString &fn, QStringList &lst);
    void updateUi();
    void enableActions(bool);
    void updateApplicationIdentity();
    void expandReccursively(const QModelIndex &index);
    void collapseReccursively(const QModelIndex &index);


	void hideDockWidgets();
	void showDockWidgets();
	bool search(const QModelIndex &index,const QString &name);
	void resetViews(bool allClear);

private:
	Ui::MainWindow ui;
	osg::ref_ptr<osg::Node> m_ptrRootNode;
	xSceneView *m_pSceneView;
	xSceneModel *m_pSceneModel;

	QList<QWidget *> m_listDock;
	xTreeModel *m_pTreeModel;
	xTreeView *m_pTreeView;

	QString m_appName;
	QString m_version;
    // variables
	QString m_currFile;
    QString m_lastDirectory;

    // recent files
    QMenu *m_pRecentFilesMenu;
    QStringList m_recentFiles;

	// for Lod factor manipulation
	QLabel *m_pLODFactorLabel;
	float m_fLODFactor;
	
	bool m_bOptimize;

	xObjectLoader *m_pObjectLoader;
    // display aspect ratio of the central windows
    QLabel *m_pAspectRatioLabel;

	// for search function
	bool m_bCaseSensitive;
	QCompleter *m_pCompleterSearch;
	QModelIndex m_currSearchIndex;
	xSearchLineEdit *m_pLineEditSearch;
	
	xPropertyWidget *m_pPropertyWidget;
};

#endif 
