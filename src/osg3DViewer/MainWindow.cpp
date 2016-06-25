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

#include "MainWindow.h"
#include "xAppSettings.h"
#include "xLogHandler.h"

#ifdef WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif
#include <QtCore/QSettings>
#include <QtCore/QProcess>
#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtCore/QUrl>
#include <QtGui/QFileDialog>
#include <QtGui/QColorDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDesktopWidget>
#include <QtGui/QLabel>
#include <QtGui/QAction>
#include <QtCore/QTextStream>
#include <QtGui/QCompleter>
#include <QtGui/QVector3D>
#include <QtGui/QMatrix4x4>
#include <QtGui/QGridLayout>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include "xSceneView.h"
#include "xSceneModel.h"
#include "xTreeModel.h"
#include "xTreeView.h"
#include "xPropertyWidget.h"
#include "xSearchLineEdit.h"

//#include "PreferencesWidget.h"
#include "xThreadPool.h"
#include "xObjectLoader.h"
//#include "SceneViewState.h"
//#include "BookmarkItem.h"
//#include "BookmarkDialog.h"
//#include "PosterImageDialog.h"
//#include "AboutDialog.h"

//#include "MiscFunctions.h"
//
#include "xFindNameListVisitor.h"

const int maxRecentlyOpenedFile = 10;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
	m_pTreeModel(NULL),
	m_pTreeView(NULL),
    //m_currentSnapshotNum(0),
    //m_lastSnapshotName("snapshot"),
    m_appName(PACKAGE_NAME),
    m_version(PACKAGE_VERSION),
    //m_recentFilesMenu(NULL),
    //m_logLevel(LogHandler::LOG_DEBUG),
    //m_splashscreenAtStartup(true),
    //m_splashscreenTransparentBackground(true),
    //m_resetConfig(false),
    //m_watermark(true),
    //m_keepCompassState(false),
    //m_inverseMouseWheel(false),
    //m_displayTrackballHelper(false),
    //m_optimize(false),
    m_pLineEditSearch(NULL),
    m_pSceneModel(NULL),
    m_pLODFactorLabel(NULL),
    m_fLODFactor(1.0f),
    m_bOptimize(false),
    m_pObjectLoader(NULL),
    m_pAspectRatioLabel(NULL),
    m_bCaseSensitive(false),
    m_pCompleterSearch(0)
    //m_typeSnapshot(SNAPSHOT_FOR_FILE),
    //m_bmkDialog(NULL),
    //m_posterDialog(NULL),
    //m_about(NULL)
{
    ui.setupUi(this);
    //loadSettings();
	
	QGridLayout* grid = new QGridLayout;
	grid->setContentsMargins(1,1,1,1);
	grid->setSpacing(0);
	ui.centralwidget->setLayout(grid);

	m_pSceneView = new xSceneView(ui.centralwidget);
	grid->addWidget(m_pSceneView, 0, 0);

	m_pPropertyWidget = new xPropertyWidget(ui.dockWidget_Properties);
	ui.dockWidget_Properties->setWidget(m_pPropertyWidget);

	loadSettings();
    updateUi();
}

MainWindow::~MainWindow()
{
	saveSettings();
}

void MainWindow::updateUi()
{
    // setup app name, release, ...
    updateApplicationIdentity();

    // prepare recent files menu ...
    connect(ui.menuFile, SIGNAL(aboutToShow()), this, SLOT(setupRecentFilesMenu()));
    connect(ui.menuRecent_Files, SIGNAL(triggered(QAction*)), this, SLOT(recentFileActivated(QAction*)));

    // create the log handler
    connect(xLogHandler::getInstance(),SIGNAL(sigNewMessage(const QString &)),this,SLOT(slotPrintToLog(const QString &)));
    connect(xLogHandler::getInstance(),SIGNAL(sigNewMessages(const QStringList &)),this,SLOT(slotPrintToLog(const QStringList &)));
    xLogHandler::getInstance()->startEmission(true); // start log emission

    // limit log display
    ui.textBrowserLog->document()->setMaximumBlockCount(1000);

    // create tree model
    m_pTreeModel = new xTreeModel(this);
	m_pTreeView = new xTreeView(ui.dockWidget_Model);
	m_pTreeView->setModel(m_pTreeModel);
	m_pSceneView->installEventFilter(this);
	connect(m_pTreeView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(slotTreeNodeSelected(const QModelIndex &)), Qt::UniqueConnection);
	connect(m_pTreeView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(slotTreeViewCustomContextMenuRequested(const QPoint &)), Qt::UniqueConnection);
	//QVBoxLayout *pTreeWidgetLayout = new QVBoxLayout(this);
	
	QWidget *pStructureWidget = new QWidget(ui.dockWidget_Model);
	ui.dockWidget_Model->setWidget(pStructureWidget);
	QVBoxLayout *pTreeLayout = new QVBoxLayout(pStructureWidget);
	pStructureWidget->setLayout(pTreeLayout);
	
	m_pLineEditSearch = new xSearchLineEdit(ui.dockWidget_Model);
	m_pLineEditSearch->setObjectName(QString::fromUtf8("lineEditSearch"));
	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(m_pLineEditSearch->sizePolicy().hasHeightForWidth());
	m_pLineEditSearch->setSizePolicy(sizePolicy);
	connect(m_pLineEditSearch, SIGNAL(sigCaseSensitiveToggled(bool)), this, SLOT(slotSearchCaseSensitiveToggled(bool)), Qt::UniqueConnection);
	connect(m_pLineEditSearch, SIGNAL(returnPressed()), this, SLOT(slotEditSearchReturnPressed()), Qt::UniqueConnection);

	pTreeLayout->addWidget(m_pLineEditSearch);
	pTreeLayout->addWidget(m_pTreeView);
	
    enableActions(false);

    // create the scene manager (scene model)
	m_pSceneModel = new xSceneModel(this);
	m_pSceneView->setModel(m_pSceneModel);
	connect(m_pSceneModel, SIGNAL(sigLoadFinished()), m_pSceneView, SLOT(slotUpdateModel()), Qt::UniqueConnection);
	
    //connect(ui->widgetSceneView,SIGNAL(newScreenshotAvailable(osg::Image *)),this,SLOT(takeIntoAccountScreenshot(osg::Image*)),Qt::QueuedConnection);
    connect(m_pSceneView, SIGNAL(sigPicked(osg::Drawable*)), this, SLOT(slotSelectTreeItem(osg::Drawable*)));
    connect(m_pSceneView, SIGNAL(sigNewAspectRatio(const QSize &)), this, SLOT(slotChangeAspectRatio(const QSize &)));

    // add a label to display the current aspect ratio of the  display (for bookmark)
    m_pAspectRatioLabel = new QLabel(this);
    QMainWindow::statusBar()->addPermanentWidget(m_pAspectRatioLabel);

    // add a label to display the LOD factor
    m_pLODFactorLabel = new QLabel(this);
    m_pLODFactorLabel->setText("LOD x1"); // set default value
    QMainWindow::statusBar()->addPermanentWidget(m_pLODFactorLabel);

    // create a background loader
    m_pObjectLoader = new xObjectLoader();
    m_pObjectLoader->moveToThread(xThreadPool::getInstance()->getThread());
    connect(this,SIGNAL(sigNewFileToLoad(const QString &)),m_pObjectLoader,SLOT(slotNewObjectToLoad(const QString &)));
    connect(m_pObjectLoader,SIGNAL(sigNewObjectToView(osg::Node *)),this,SLOT(slotNewLoadedFile(osg::Node *)));

    //// bookmarks
    //connect(ui->widgetBookmarkManager,SIGNAL(newBookmarkRequest()),this,SLOT(on_actionNewBookmark_triggered()));
    //connect(ui->widgetBookmarkManager,SIGNAL(useBookmarkItem(BookmarkItem *)),this,SLOT(useBookmarkItem(BookmarkItem *)));
    //connect(ui->widgetBookmarkManager,SIGNAL(updateBookmarkItemRequest(BookmarkItem *)),this,SLOT(updateBookmarkItem(BookmarkItem *)));
}

void MainWindow::on_actionQuit_triggered()
{
    close();
}

void MainWindow::on_actionOpen_triggered()
{
    QString file = QFileDialog::getOpenFileName(this, "Select one file to open", m_lastDirectory, "OSG files (*.*)");
    loadFile(file);
}
void MainWindow::on_actionSave_As_triggered()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), m_lastDirectory, tr("OSG files (*.*)"));

	m_pSceneModel->saveSceneData(fileName.toStdString());
	//osgDB::writeNodeFile(*m_rootNode.get(), fileName.toStdString());
}

bool MainWindow::loadFile(const QString &file)
{
	if (file.isEmpty() || !QFileInfo(file).exists())
		return false;

	//saveIfNeeded();

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	m_currFile = file;
	m_lastDirectory = QFileInfo(file).absolutePath();

	// enable actions
	enableActions(false);

	emit sigNewFileToLoad(file);

	return true;
}

void MainWindow::slotNewLoadedFile(osg::Node *node)
{
	// reset the cursor
	QApplication::restoreOverrideCursor();

	resetViews(true);

	if (!node)
	{
		return;
	}

	m_pSceneModel->setData(node);

	// update the window title
	setWindowTitle(m_appName + " " + m_version + " - " + QFileInfo(m_currFile).fileName());
	addRecentlyOpenedFile(m_currFile, m_recentFiles);

	// reset the cursor
	QApplication::restoreOverrideCursor();

	// enable actions
	enableActions(true);

	m_pTreeModel->setNode(node);

	// arrange display
	QModelIndex i = m_pTreeModel->index(0, 0, m_pTreeView->rootIndex());
	m_pTreeView->setExpanded(i,true);
	m_pTreeView->resizeColumnToContents(0);

	// update completer for find
	delete m_pCompleterSearch;

	xFindNameListVisitor visit;
	node->accept(visit);

	QStringList nameList = visit.getNameList();
	nameList.removeDuplicates();

	m_pCompleterSearch = new QCompleter(nameList, this);
	m_pLineEditSearch->setCompleter(m_pCompleterSearch);
	if (m_bCaseSensitive)
		m_pCompleterSearch->setCaseSensitivity(Qt::CaseSensitive);
	else
		m_pCompleterSearch->setCaseSensitivity(Qt::CaseInsensitive);

	// display stats
	m_pPropertyWidget->displayProperties(node);

	// init current index
	m_currSearchIndex = QModelIndex();

	//// check for bookmark file
	//QFileInfo fi(m_currFile);
	//QFileInfo bmk(fi.absolutePath() + "/" + fi.baseName() + ".bmk");
	//if (bmk.exists())
	//{
	//	if (ui->widgetBookmarkManager->loadBookmark(bmk.absoluteFilePath()))
	//	{
	//		LogHandler::getInstance()->reportInfo(tr("Loading of %1 ...").arg(bmk.absoluteFilePath()));
	//	}
	//	else
	//	{
	//		LogHandler::getInstance()->reportInfo(tr("Nothing to load in %1 ...").arg(bmk.absoluteFilePath()));
	//	}
	//}
}

void MainWindow::addRecentlyOpenedFile(const QString &fn, QStringList &lst)
{
    QFileInfo fi(fn);

    if (lst.contains(fi.absoluteFilePath()))
        return;

    if (lst.count() >= maxRecentlyOpenedFile)
        lst.removeLast();

    lst.prepend(fi.absoluteFilePath());
}

void MainWindow::setupRecentFilesMenu()
{
    ui.menuRecent_Files->clear();

    if (m_recentFiles.count() > 0)
    {
        ui.menuRecent_Files->setEnabled(true);
        QStringList::Iterator it = m_recentFiles.begin();

        for (; it != m_recentFiles.end(); ++it)
             ui.menuRecent_Files->addAction(*it);
    }
    else
    {
         ui.menuRecent_Files->setEnabled(false);
    }
}

void MainWindow::recentFileActivated(QAction *action)
{
    if (!action->text().isEmpty())
    {
        if (!loadFile(action->text()))
			m_recentFiles.removeAt(m_recentFiles.indexOf(action->text()));
    }
}

// enable/disable actions depending on a valid loaded file
void MainWindow::enableActions(bool val)
{
    //ui->actionSnapShot->setEnabled(val);
    ui.actionReset_View->setEnabled(val);
    ui.actionTexture->setEnabled(val);
    ui.actionLight->setEnabled(val);
    ui.actionHighLight->setEnabled(val);
    ui.actionBackFace->setEnabled(val);
    ui.actionOptimize->setEnabled(val);
    //ui->actionIncreaseLOD->setEnabled(val);
    //ui->actionDecreaseLOD->setEnabled(val);
    //ui->actionResetLOD->setEnabled(val);
    //ui->actionNewBookmark->setEnabled(val);

    //ui->widgetBookmarkManager->setEnabled(val);
    //ui->widgetStats->setEnabled(val);
    ui.dockWidget_Model->setEnabled(val);
    ui.dockWidget_Properties->setEnabled(val);
}


void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData* mimeData = event->mimeData();

    if (!mimeData->hasUrls())
    {
        event->ignore();
        return;
    }

    QList<QUrl> urls = mimeData->urls();

    if(urls.count() != 1)
    {
        event->ignore();
        return;
    }

    QUrl url = urls.at(0);
    QString filename = url.toLocalFile();

    // We don't test extension
    if (!QFileInfo(filename).exists())
    {
        event->ignore();
        return;
    }

    if (!QFileInfo(filename).isFile())
    {
        event->ignore();
        return;
    }

    event->acceptProposedAction();
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->source() == this)
    {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
    else
    {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData* mimeData = event->mimeData();
    QList<QUrl> urls = mimeData->urls();
    QUrl url = urls.at(0);

    QString filename = url.toLocalFile();
    loadFile(filename);
}

//-------------------------------------------------------------------------------
void MainWindow::closeEvent(QCloseEvent *event)
{
    //if (!saveIfNeeded())
    //{
    //    statusBar()->showMessage(tr("Quit application aborted !!"), 5000);
    //    event->ignore();
    //    return; // cancel triggered !!
    //}

     //just in case => restore the dockWidgets
    showDockWidgets();
}

void MainWindow::updateApplicationIdentity()
{
	QString strTitle = PACKAGE_NAME;
	strTitle += " ";
	strTitle += PACKAGE_VERSION;
    setWindowTitle(strTitle);
    QApplication::setApplicationName(PACKAGE_NAME);
    QApplication::setApplicationVersion(PACKAGE_VERSION);
    QApplication::setOrganizationName(PACKAGE_ORGANIZATION);
}

void MainWindow::on_actionUnload_triggered()
{
    //if (!saveIfNeeded())
    //{
    //    statusBar()->showMessage(tr("Close current object aborted !!"), 5000);
    //    return; // cancel triggered !!
    //}

    m_pSceneModel->setData(NULL);
	m_pTreeModel->setNode(NULL);

    // disable actions because no more current file !!
    enableActions(false);
}

void MainWindow::saveSettings()
{
	QSettings settings(PACKAGE_ORGANIZATION, PACKAGE_NAME);

	settings.beginGroup("MainWindow");
	settings.setValue("MainWindowState",saveState(0));
	settings.setValue("size", size());
	settings.setValue("pos", pos());
	settings.setValue("fullScreen", isFullScreen());
	settings.endGroup();

	settings.beginGroup("Application");

	settings.setValue("lastDirectory", m_lastDirectory);
	//settings.setValue("lastDirectorySnapshot", m_lastDirectorySnapshot);

	//settings.setValue("currentLanguage", m_currentLanguage);

	//settings.setValue("resetConfig", m_resetConfig);
	//settings.setValue("watermark", m_watermark);
	//settings.setValue("inverseMouseWheel", m_inverseMouseWheel);
	//settings.setValue("displayTrackballHelper", m_displayTrackballHelper);

	//settings.setValue("splashscreenAtStartup", m_splashscreenAtStartup);
	//settings.setValue("splashscreenTransparentBackground", m_splashscreenTransparentBackground);

	// recent files
	settings.setValue("recentlyOpenedFiles", m_recentFiles);

	// scene background
	//settings.setValue("bgcolor", ui->widgetSceneView->getBgColor());

	settings.endGroup();
}

void MainWindow::loadSettings()
{
	xAppSettings settings;

	settings.beginGroup("MainWindow");

	restoreState(settings.value("MainWindowState").toByteArray(), 0);

	resize(settings.value("size", QSize(600, 600)).toSize());
	move(settings.value("pos", QPoint(200, 200)).toPoint());

	bool fullScreen = settings.value("fullScreen",false).toBool();
	if (fullScreen)
		showFullScreen();
	settings.endGroup();

	settings.beginGroup("Application");
	m_lastDirectory = settings.value("lastDirectory","/home").toString();
	//m_lastDirectorySnapshot = settings.value("lastDirectorySnapshot","/home").toString();

	// recent files
	m_recentFiles = settings.value("recentlyOpenedFiles").toStringList();

	//m_splashscreenAtStartup = settings.value("splashscreenAtStartup", true).toBool();
	//m_splashscreenTransparentBackground = settings.value("splashscreenTransparentBackground",true).toBool();

	//m_resetConfig = settings.value("resetConfig", false).toBool();
	//m_watermark = settings.value("watermark", true).toBool();
	//m_inverseMouseWheel = settings.value("inverseMouseWheel", false).toBool();
	//ui->widgetSceneView->setEnabledInverseMouseWheel(m_inverseMouseWheel);

	//m_displayTrackballHelper = settings.value("displayTrackballHelper", false).toBool();
	//ui->widgetSceneView->setEnabledTrackbalHelper(m_displayTrackballHelper);

	//m_currentLanguage = settings.value("currentLanguage","").toString();

	//if (m_currentLanguage.isEmpty())
	//	m_currentLanguage = QLocale::system().name().left(2);

	//// scene background
	//QColor color = settings.value("bgcolor",QColor(50,50,50)).value<QColor>();
	//ui->widgetSceneView->setBgColor(color);

	settings.endGroup();
}

void MainWindow::on_actionReset_View_triggered()
{
	m_pSceneView->home();
}
void MainWindow::on_actionFull_Screen_triggered(bool on)
{
	if (on)
	{
		setWindowState(windowState() | Qt::WindowFullScreen);
		hideDockWidgets();
	}
	else
	{
		setWindowState(windowState() & ~Qt::WindowFullScreen);
		showDockWidgets();
	}
}

void MainWindow::on_actionShadow_triggered(bool val)
{
	m_pSceneView->setShadowEnabled(val);
}

void MainWindow::on_actionTexture_triggered(bool val)
{
	//if (val)
	//{
	//	m_keepCompassState = ui->widgetSceneView->isCompassEnabled();
	//	ui->widgetSceneView->setCompassEnabled(false);
	//}
	//else
	//{
	//	ui->widgetSceneView->setCompassEnabled(m_keepCompassState);
	//}

	//ui->actionCompass->setEnabled(!val);
	m_pSceneView->setTextureEnabled(val);
}

void MainWindow::on_actionLight_triggered(bool val)
{
	m_pSceneView->setLightingEnabled(val);
}

void MainWindow::on_actionHighLight_triggered(bool val)
{
	m_pSceneView->setHighlightScene(val);
}
void MainWindow::on_pushButtonClearLog_pressed()
{
	ui.textBrowserLog->clear();
}

void MainWindow::on_pushButtonSaveLog_pressed()
{
	QString logFile = QFileDialog::getSaveFileName(
		this,
		tr("Log file"),
		m_lastDirectory,
		"Log file (*.log)");

	if (logFile.isEmpty())
		return;

	m_lastDirectory = QFileInfo(logFile).absolutePath(); // save current directory

	QFile file(logFile);

	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		xLogHandler::getInstance()->slotReportError(tr("Can't save log file !!: %1").arg(logFile));
	}

	QTextStream out(&file);
	out << ui.textBrowserLog->document()->toPlainText();
	file.close();
}
void MainWindow::on_actionBackFace_triggered(bool val)
{
    m_pSceneView->setBackfaceEnabled(!val);
}
void MainWindow::on_actionOptimize_triggered(bool val)
{
    m_bOptimize = val;
    if (m_pObjectLoader != NULL)
    {
        m_pObjectLoader->slotSetOptimization(m_bOptimize);
    }
}

void MainWindow::on_actionEnableNode_triggered()
{
    //qDebug("on_actionEnableNode_activated");
    QModelIndex index = m_pTreeView->currentIndex();

    if ( !index.isValid() )
        return;

    m_pTreeModel->setEnableIndex(index,true);
    m_pTreeView->update();
}

void MainWindow::on_actionDisableNode_triggered()
{
    QModelIndex index = m_pTreeView->currentIndex();

    if ( !index.isValid() )
        return;

    m_pTreeModel->setEnableIndex(index,false);
    m_pTreeView->update();
}

void MainWindow::on_actionExpandTree_triggered()
{
    QModelIndex index = m_pTreeView->currentIndex();

    if ( !index.isValid() )
        return;

    expandReccursively(index);
    m_pTreeView->resizeColumnToContents(0);
}

void MainWindow::on_actionCollapseTree_triggered()
{
    QModelIndex index = m_pTreeView->currentIndex();

    if ( !index.isValid() )
        return;

    collapseReccursively(index);
}

void MainWindow::expandReccursively(const QModelIndex &index)
{
    if ( !index.isValid() )
        return;

    for (int i = 0; i < m_pTreeModel->rowCount(index); i++)
    {
        QModelIndex ind = index.child(i,0);
        expandReccursively(ind);
    }

    m_pTreeView->expand(index);
}

void MainWindow::collapseReccursively(const QModelIndex &index)
{
    if ( !index.isValid() )
        return;

    m_pTreeView->collapse(index);

    for (int i = 0; i < m_pTreeModel->rowCount(index); i++)
    {
        QModelIndex ind = index.child(i,0);
        collapseReccursively(ind);
    }
}
void MainWindow::on_actionCenterOnSelection_triggered()
{
    QModelIndex index = m_pTreeView->currentIndex();

    if ( !index.isValid() )
        return;

    m_pSceneView->centerOnNode( reinterpret_cast<osg::Node*>( index.internalPointer() ) );

    // TODO !!!!
}

void MainWindow::slotTreeViewCustomContextMenuRequested(const QPoint & pos)
{
    QModelIndex index = m_pTreeView->indexAt(pos);

    if ( !index.isValid() )
        return;

    QMenu menu(this);

    menu.addAction(ui.actionEnableNode);
    menu.addAction(ui.actionDisableNode);
    menu.addAction(ui.actionCenterOnSelection);
    menu.insertSeparator(0);
    menu.addAction(ui.actionExpandTree);
    menu.addAction(ui.actionCollapseTree);

    menu.exec( m_pTreeView->viewport()->mapToGlobal(pos) );
}

void MainWindow::showDockWidgets()
{
	QMainWindow::menuBar()->show();
	QMainWindow::statusBar()->show();

	// restore the dock widget state
	foreach(QWidget * widget, m_listDock)
	{
		widget->show();
	}
	m_listDock.clear();
}

void MainWindow::hideDockWidgets()
{
	QMainWindow::statusBar()->hide();
	QMainWindow::menuBar()->hide();

	// save the visible dock widget
	foreach(QWidget *widget, findChildren<QDockWidget*>())
	{
		if (widget->isVisible())
		{
			m_listDock << widget;
			widget->hide();
		}
	}
}

void MainWindow::slotTreeNodeSelected(const QModelIndex &index)
{
	if (index.isValid())
	{
		m_pSceneView->highlight((osg::Node *)index.internalPointer());
		// display Properties
		//m_pSceneView->displayProperties((osg::Node *)index.internalPointer());
		m_pPropertyWidget->displayProperties((osg::Node *)index.internalPointer());
	}
}

void MainWindow::slotSelectTreeItem(osg::Drawable *pDrawable)
{
	QModelIndex parentIndex = m_pTreeModel->searchForNode(pDrawable->getParent(0));

	if (parentIndex.isValid())
	{
		m_pTreeView->setCurrentIndex(parentIndex);
		m_pTreeView->scrollTo(parentIndex, QAbstractItemView::EnsureVisible);
		m_pTreeView->resizeColumnToContents(0);
		slotTreeNodeSelected(parentIndex);
	}
}

void MainWindow::resetViews(bool allClear)
{
	// clear current bookmark
	//ui->widgetBookmarkManager->clearBookmark();

	m_pSceneView->resetSelection();

	// reset 3d view
	if (allClear)
	{
		m_pTreeModel->setNode(NULL);
		m_pSceneModel->setData(NULL);
	}
}
void MainWindow::slotPrintToLog(const QString & mess)
{
	ui.textBrowserLog->append(mess);
}

void MainWindow::slotPrintToLog(const QStringList & mess)
{
	ui.textBrowserLog->append(mess.join("<br>"));
}
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
	if (obj == m_pSceneView)
	{
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		keyPressEvent(keyEvent);
	}

	return false;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
	if(event->key() == Qt::Key_Escape)
	{
		if (bool(windowState() & Qt::WindowFullScreen) == true) // fullscreen mode => quit fullscreen
		{
			on_actionFull_Screen_triggered(false);
			ui.actionFull_Screen->setChecked(false);
		}

		return;
	}

	bool shift = event->modifiers() & Qt::ShiftModifier;
	bool ctrl = event->modifiers() & Qt::ControlModifier;
	bool alt = event->modifiers() & Qt::AltModifier;

	if (shift)
	{
		QMainWindow::statusBar()->showMessage(tr("Use Shift+Left Click to recenter view"), 5000);
	}
	else if (ctrl)
	{
		QMainWindow::statusBar()->showMessage(tr("Use Ctrl+Left Click to select element"), 5000);
	}
}
void MainWindow::slotChangeAspectRatio(const QSize & sz)
{
	if (sz.height())
	{
		float ratio = sz.width() / (float)sz.height();
		m_pAspectRatioLabel->setText(tr("View aspect ratio: %1").arg(QString::number(ratio,'g',2)));
	}
}
void MainWindow::slotSearchCaseSensitiveToggled(bool checked)
{
	m_bCaseSensitive = checked;

	if (!m_pCompleterSearch)
		return;

	if (m_bCaseSensitive)
		m_pCompleterSearch->setCaseSensitivity(Qt::CaseSensitive);
	else
		m_pCompleterSearch->setCaseSensitivity(Qt::CaseInsensitive);
}

void MainWindow::slotEditSearchReturnPressed()
{
	QString searchText = m_pLineEditSearch->text();

	if  (!m_currSearchIndex.isValid())
	{
		m_currSearchIndex = m_pTreeModel->index(0, 0, QModelIndex());
		search(m_currSearchIndex,searchText);
	}
	else
	{
		// search for next valid sibling or root
		QModelIndex node = m_currSearchIndex;
		QModelIndex sibling;
		bool found = false;
		while (!found)
		{
			sibling = node.sibling(node.row() + 1,node.column());
			while (!sibling.isValid())
			{
				node = node.parent();
				if (!node.isValid())
				{
					found = true;
					break;
				}
				if (node != m_pTreeModel->index(0, 0, QModelIndex()))
					sibling = node.sibling(node.row() + 1,node.column());
				else
					sibling = m_pTreeModel->index(0, 0, QModelIndex());
			}
			if (found)
				break;
			found = search(sibling,searchText);
			node = sibling;
		}
	}
}

bool MainWindow::search(const QModelIndex &index,const QString &name)
{
	m_currSearchIndex = m_pTreeModel->searchForName(name,index);
	if (m_currSearchIndex.isValid())
	{
		m_pTreeView->setCurrentIndex(m_currSearchIndex);
		m_pTreeView->scrollTo(m_currSearchIndex,QAbstractItemView::EnsureVisible);
		m_pTreeView->resizeColumnToContents(0);

		//widgetProperties->displayProperties(curritf);

		slotTreeNodeSelected(m_currSearchIndex);
		m_pSceneView->centerOnNode(reinterpret_cast<osg::Node*>(m_currSearchIndex.internalPointer()));
		return true;
	}
	return false;
}
