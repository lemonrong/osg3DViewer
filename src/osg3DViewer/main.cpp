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

#include <QtGui/QApplication>
#include <QtGui/QPlastiqueStyle>
#include <QtGui/QDesktopWidget>
#include <QtCore/QTextStream>

#include <osgDB/Registry>

#include "MainWindow.h"
//#include "AppSettings.h"
//#include "MiscFunctions.h"
//#include "SplashScreen.h"

#ifdef Q_OS_WIN
#include <windows.h> // for Sleep
#endif

void usage()
{
    QTextStream out(stdout);
    out << endl;
    out << QObject::tr("OSG viewer") << endl;
    out << QObject::tr("Usage: ") << QCoreApplication::arguments().at(0) << " " << QObject::tr("[options]") << " " << QObject::tr("filename") << endl;
    out << endl;
    out << "Following options are known:" << endl;
    out << QObject::tr(" --help                 - displays this help.") << endl;
    out << QObject::tr(" --reset-config         - clear the saved preference parameters.") << endl;
    out << endl;
}

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(osg3DViewer);

    QApplication app(argc, argv);

    // add the current path
    osgDB::Registry::instance()->getDataFilePathList().push_back( qApp->applicationDirPath().toStdString () );
    osgDB::Registry::instance()->getDataFilePathList().push_back( QString(qApp->applicationDirPath() + "/data/").toStdString () );
    osgDB::Registry::instance()->getDataFilePathList().push_back( QString(qApp->applicationDirPath() + "/../data/").toStdString () );
    osgDB::Registry::instance()->getDataFilePathList().push_back( QString(qApp->applicationDirPath() + "/../../data/").toStdString () );
    osgDB::Registry::instance()->getDataFilePathList().push_back( "/usr/local/share/osgrafx/data/" );
    osgDB::Registry::instance()->getDataFilePathList().push_back( "/usr/share/osgrafx/data/" );

    // in order to speed up the picking process ...
    osgDB::Registry::instance()->setBuildKdTreesHint(osgDB::ReaderWriter::Options::BUILD_KDTREES);

    //MiscFunctions::setDefaultLanguage();

    QApplication::setStyle(new QPlastiqueStyle);

    // check for special argument
    bool forceResetConfig = false;
    QString filename;
    QStringList args = QApplication::arguments();
    for ( int i = 1; i < args.count(); ++i )
    {
        const QString arg = args.at(i);

        if ( arg == "--reset-config" )
        {
            forceResetConfig = true;
        }
        else if (arg == "--help")
        {
            usage();
            return 0;
        }
        else
        {
            filename = arg;
        }
    }

    //AppSettings settings;

    //// in order to display splashscreen on the same screen than application ...
    //settings.beginGroup("MainWindow");
    //int screenNumber = settings.value("screenNumber",0).toInt();
    //settings.endGroup();

    //settings.beginGroup("Application");

    //// reset the saved configuration if needed
    //bool resetConfig = settings.value("resetConfig",false).toBool();
    //if (resetConfig || forceResetConfig)
    //{
    //    settings.clear();
    //    settings.sync();
    //}

    //bool splashscreenAtStartup = settings.value("splashscreenAtStartup",true).toBool();
    //bool splashscreenTransparentBackground = settings.value("splashscreenTransparentBackground",true).toBool();
    //settings.endGroup();

    //// splash screen
    //if (splashscreenAtStartup)
    //{
    //    SplashScreen sScreen(QPixmap(":/osGraphX/splashscreen.png"), 3000, screenNumber,splashscreenTransparentBackground);
    //    sScreen.show();
    //}

    //MainWindow w;
    //QObject::connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    //w.show();

    //w.loadFile(filename);

    return app.exec();
}
