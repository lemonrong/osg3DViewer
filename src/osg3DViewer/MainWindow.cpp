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
//#include "AppSettings.h"
//#include "LogHandler.h"

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
#include "xTreeModel.h"
#include "xTreeView.h"

//#include "PreferencesWidget.h"
//#include "ThreadPool.h"
//#include "ObjectLoader.h"
//#include "SceneViewState.h"
//#include "BookmarkItem.h"
//#include "BookmarkDialog.h"
//#include "PosterImageDialog.h"
//#include "AboutDialog.h"

//#include "MiscFunctions.h"
//
//#include "FindNameListVisitor.h"

#define PACKAGE_ORGANIZATION "Lemon Rong"
#define PACKAGE_NAME "osg3DViewer"
#define PACKAGE_VERSION "1.0.0"

const int maxRecentlyOpenedFile = 10;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
	m_pTreeModel(NULL),
	m_pTreeView(NULL)
    //m_currentSnapshotNum(0),
    //m_lastSnapshotName("snapshot"),
    //m_appName(PACKAGE_NAME),
    //m_version(PACKAGE_VERSION),
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
    
    //m_sceneModel(NULL),
    //m_LODFactorLabel(NULL),
    //m_LODFactor(1.0f),
    //m_prefs(NULL),
    //m_bgLoader(NULL),
    //m_aspectRatioLabel(NULL),
    //m_caseSensitive(false),
    //m_completerSearch(0),
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

    //// create the log handler
    //connect( LogHandler::getInstance(),SIGNAL( newMessage(const QString &) ),this,SLOT( printToLog(const QString &) ) );
    //connect( LogHandler::getInstance(),SIGNAL( newMessages(const QStringList &) ),this,SLOT( printToLog(const QStringList &) ) );
    //LogHandler::getInstance()->startEmission(true); // start log emission

    //// limit log display
    //ui->textBrowserLog->document()->setMaximumBlockCount(1000);

    // create tree model
    m_pTreeModel = new xTreeModel(this);
	m_pTreeView = new xTreeView(this);
	m_pTreeView->setModel(m_pTreeModel);
	//QVBoxLayout *pTreeWidgetLayout = new QVBoxLayout(this);
	
	ui.dockWidget_Model->setWidget(m_pTreeView);
	//pTreeWidgetLayout->addWidget(m_pTreeView);
    //connect( ui->treeViewStructure, SIGNAL( clicked ( const QModelIndex &) ), this, SLOT( nodeSelected(const QModelIndex & ) ) );

    //enableActions(false);

    //// create the scene manager (scene model)
    //m_sceneModel = new SceneModel(this);
    //ui->widgetSceneView->setModel(m_sceneModel);

    //connect(ui->widgetSceneView,SIGNAL( newScreenshotAvailable(osg::Image *) ),this,SLOT( takeIntoAccountScreenshot(osg::Image*) ),Qt::QueuedConnection);
    //connect( ui->widgetSceneView, SIGNAL( picked(osg::Drawable*) ), this, SLOT( selectTreeItem(osg::Drawable*) ) );
    //connect( ui->widgetSceneView, SIGNAL( newAspectRatio(const QSize &) ), this, SLOT( changeAspectRatio(const QSize &) ) );

    //// install eventfilter for sceneview
    //ui->widgetSceneView->installEventFilter(this);

    //// add a label to display the current aspect ratio of the  display (for bookmark)
    //m_aspectRatioLabel = new QLabel(this);
    //QMainWindow::statusBar()->addPermanentWidget(m_aspectRatioLabel);

    //// add a label to display the LOD factor
    //m_LODFactorLabel = new QLabel(this);
    //m_LODFactorLabel->setText("LOD x1"); // set default value
    //QMainWindow::statusBar()->addPermanentWidget(m_LODFactorLabel);

    //// create a background loader
    //m_bgLoader = new ObjectLoader();
    //m_bgLoader->moveToThread( ThreadPool::getInstance()->getThread() );
    //connect( this,SIGNAL( newFileToLoad(const QString &) ),m_bgLoader,SLOT( newObjectToLoad(const QString &) ) );
    //connect( m_bgLoader,SIGNAL( newObjectToView(osg::Node *) ),this,SLOT( newLoadedFile(osg::Node *) ) );

    //// bookmarks
    //connect( ui->widgetBookmarkManager,SIGNAL( newBookmarkRequest() ),this,SLOT( on_actionNewBookmark_triggered() ) );
    //connect( ui->widgetBookmarkManager,SIGNAL( useBookmarkItem(BookmarkItem *) ),this,SLOT( useBookmarkItem(BookmarkItem *) ) );
    //connect( ui->widgetBookmarkManager,SIGNAL( updateBookmarkItemRequest(BookmarkItem *) ),this,SLOT( updateBookmarkItem(BookmarkItem *) ) );
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

	osgDB::writeNodeFile(*m_rootNode.get(), fileName.toStdString());
}
bool MainWindow::loadFile(const QString &file)
{
    if ( file.isEmpty() || !QFileInfo(file).exists() )
        return false;

	m_rootNode = osgDB::readNodeFile(file.toStdString());
	m_pSceneView->setSceneData(m_rootNode);
	m_pTreeModel->setNode(m_rootNode);
    //saveIfNeeded();

    //QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );

    m_currFile = file;
    m_lastDirectory = QFileInfo(file).absolutePath();
	addRecentlyOpenedFile(m_currFile, m_recentFiles);
	setupRecentFilesMenu();
    //// enable actions
    //enableActions(false);

    //emit newFileToLoad(file);

    return true;
}

void MainWindow::addRecentlyOpenedFile(const QString &fn, QStringList &lst)
{
    QFileInfo fi(fn);

    if ( lst.contains( fi.absoluteFilePath() ) )
        return;

    if ( lst.count() >= maxRecentlyOpenedFile )
        lst.removeLast();

    lst.prepend( fi.absoluteFilePath() );
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
        loadFile(action->text());
    }
}

// enable/disable actions depending on a valid loaded file
void MainWindow::enableActions(bool val)
{
    //ui->actionSnapShot->setEnabled(val);
    //ui->actionResetView->setEnabled(val);
    //ui->actionTexture->setEnabled(val);
    //ui->actionLight->setEnabled(val);
    //ui->actionHighLight->setEnabled(val);
    //ui->actionBackFace->setEnabled(val);
    //ui->actionIncreaseLOD->setEnabled(val);
    //ui->actionDecreaseLOD->setEnabled(val);
    //ui->actionResetLOD->setEnabled(val);
    //ui->actionNewBookmark->setEnabled(val);

    //ui->widgetBookmarkManager->setEnabled(val);
    //ui->widgetStats->setEnabled(val);
    //ui->widgetProperties->setEnabled(val);
    //ui->treeDockContents->setEnabled(val);
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
    //if ( !saveIfNeeded() )
    //{
    //    statusBar()->showMessage( tr( "Quit application aborted !!" ), 5000 );
    //    event->ignore();
    //    return; // cancel triggered !!
    //}

    // just in case => restore the dockWidgets
    //showDockWidgets();
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
    //if ( !saveIfNeeded() )
    //{
    //    statusBar()->showMessage( tr( "Close current object aborted !!" ), 5000 );
    //    return; // cancel triggered !!
    //}

    m_pSceneView->setSceneData(NULL);

    //// disable actions because no more current file !!
    //enableActions(false);
}

void MainWindow::saveSettings()
{
	QSettings settings(PACKAGE_ORGANIZATION, PACKAGE_NAME);

	settings.beginGroup("MainWindow");
	settings.setValue( "MainWindowState",saveState(0) );
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
	//settings.setValue( "bgcolor", ui->widgetSceneView->getBgColor() );

	settings.endGroup();
}

void MainWindow::loadSettings()
{
	QSettings settings(PACKAGE_ORGANIZATION, PACKAGE_NAME);

	settings.beginGroup("MainWindow");

	restoreState(settings.value("MainWindowState").toByteArray(), 0);

	resize(settings.value( "size", QSize(600, 600)).toSize());
	move(settings.value( "pos", QPoint(200, 200)).toPoint());

	bool fullScreen = settings.value("fullScreen",false).toBool();
	if (fullScreen)
		showFullScreen();
	settings.endGroup();

	settings.beginGroup("Application");
	m_lastDirectory = settings.value("lastDirectory","/home").toString();
	//m_lastDirectorySnapshot = settings.value("lastDirectorySnapshot","/home").toString();

	// recent files
	m_recentFiles = settings.value( "recentlyOpenedFiles").toStringList();

	//m_splashscreenAtStartup = settings.value( "splashscreenAtStartup", true).toBool();
	//m_splashscreenTransparentBackground = settings.value("splashscreenTransparentBackground",true).toBool();

	//m_resetConfig = settings.value("resetConfig", false).toBool();
	//m_watermark = settings.value("watermark", true).toBool();
	//m_inverseMouseWheel = settings.value("inverseMouseWheel", false).toBool();
	//ui->widgetSceneView->setEnabledInverseMouseWheel(m_inverseMouseWheel);

	//m_displayTrackballHelper = settings.value("displayTrackballHelper", false).toBool();
	//ui->widgetSceneView->setEnabledTrackbalHelper(m_displayTrackballHelper);

	//m_currentLanguage = settings.value("currentLanguage","").toString();

	//if ( m_currentLanguage.isEmpty() )
	//	m_currentLanguage = QLocale::system().name().left(2);

	//// scene background
	//QColor color = settings.value( "bgcolor",QColor(50,50,50) ).value<QColor>();
	//ui->widgetSceneView->setBgColor(color);

	settings.endGroup();
}
