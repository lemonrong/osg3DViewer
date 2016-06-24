
/******************************************************************************
   osGraphX: a 3D file viewer
   Copyright(C) 2011-2012  xbee@xbee.net

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *******************************************************************************/

#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QMutexLocker>
#include <QtCore/QFileInfo>

#include <stdlib.h> // for getenv

#include "xLogHandler.h"
#include "xAppSettings.h"

const QString logHeaderInformation("GraphX Log");

const int sendMessageInterval = 500; // 500 ms

xLogHandler* xLogHandler::m_pInstance = 0;

xLogHandler *xLogHandler::getInstance()
{
    static QMutex mutex;

    if (!m_pInstance)
    {
        mutex.lock();

        if (!m_pInstance)
            m_pInstance = new xLogHandler;

        mutex.unlock();
    }

    return m_pInstance;
}

xLogHandler::~xLogHandler()
{
    static QMutex mutex;
    mutex.lock();
    m_pInstance = 0;
    mutex.unlock();
}

xLogHandler::xLogHandler() :
    m_currLevel(LOG_DEBUG),
    m_bBufferized(true),
    m_bStartEmission(false),
    m_pTimer(NULL),
    m_bSaveToLog(false)
{
    loadSettings();
    m_pTimer = new QTimer(this);
    m_pTimer->setInterval(sendMessageInterval);
    connect(m_pTimer,SIGNAL(timeout()),this,SLOT(slotUnqueueWaitingMessages()));
}

void xLogHandler::reportMessage(LogLevel level, const QString &message)
{
    // message Ignored
    if (level < m_currLevel)
        return;

    /* Firstly send message to file log */
    if (m_bSaveToLog)
        fillAppLogFile(message);

    QString msgText(message);
    msgText.replace("<", "&lt;");
    msgText.replace(">", "&gt;");

    // add the current time
    msgText = QTime::currentTime().toString("[hh:mm:ss]: ") + msgText;
    QString msg;

    switch (level)
    {
        case LOG_DEBUG:
        {
            msg = QString("<font color=\"blue\"><b>DEBUG</b></font>: ") + msgText;
            break;
        }
        case LOG_INFO:
        {
            msg = QString("<font color=\"green\"><b>INFO</b>: </font>") + msgText;
            break;
        }
        case LOG_WARNING:
        {
            msg = QString("<font color=\"orange\"><b>WARNING</b>: </font>") + msgText;
            break;
        }
        case LOG_ERROR:
        {
            msg = QString(" <font color=\"red\"><b>ERROR</b>:</font>") + msgText;
            break;
        }
        default:
        {
            msg = msgText;
            break;
        }
    }

    if (m_bBufferized || !m_bStartEmission)
        m_buffer << msg;
    else
        emit sigNewMessage(msg);
}

void xLogHandler::slotReportDebug(const QString &message)
{
    QMutexLocker locker(&m_lock);
    reportMessage(LOG_DEBUG,message);
}

void xLogHandler::slotReportInfo(const QString &message)
{
    QMutexLocker locker(&m_lock);
    reportMessage(LOG_INFO,message);
}

void xLogHandler::slotReportWarning(const QString &message)
{
    QMutexLocker locker(&m_lock);
    reportMessage(LOG_WARNING,message);
}

void xLogHandler::slotReportError(const QString &message)
{
    QMutexLocker locker(&m_lock);
    reportMessage(LOG_ERROR,message);
}

void xLogHandler::setBufferization(bool val)
{
    m_bBufferized = val;
    if (!m_bBufferized) // send all stored messages
    {
        m_pTimer->stop();
        slotUnqueueWaitingMessages();
    }
    else
    {
        m_pTimer->start();
    }
}

void xLogHandler::startEmission(bool val)
{
    m_bStartEmission = val;
    if (m_bStartEmission && m_bBufferized)
        m_pTimer->start();
    else
        m_pTimer->stop();
}

void xLogHandler::slotUnqueueWaitingMessages()
{
    QMutexLocker locker(&m_lock);
    if (m_bStartEmission)
    {
        if (!m_buffer.isEmpty())
        {
            emit sigNewMessages(m_buffer);
            m_buffer.clear();
        }
    }
}

void xLogHandler::loadSettings()
{
    xAppSettings settings;

    settings.beginGroup("Application");
    settings.beginGroup("Log");

    m_logDirectory = settings.value("logDirectory","/usr/tmp").toString();

    settings.endGroup();
    settings.endGroup();
}

void xLogHandler::setLogDirectory(const QString &path)
{
    if (m_logDirectory == path || !QFileInfo(path).exists())
        return;

    m_logDirectory = path;
    if (m_bSaveToLog && m_pLogFile)
    {
        m_pLogFile->close();
        delete m_pLogStream;
        m_pLogStream = NULL;
        delete m_pLogFile;
        m_pLogFile = NULL;
    }
}

void xLogHandler::setLogToFile(bool val)
{
    if (m_bSaveToLog == val)
        return;

    m_bSaveToLog = val;
    if (!m_bSaveToLog && m_pLogFile)
    {
        m_pLogFile->close();
        delete m_pLogStream;
        m_pLogStream = NULL;
        delete m_pLogFile;
        m_pLogFile = NULL;
    }
}

bool xLogHandler::fillAppLogFile(const QString &message)
{
    /* Assure one access on the file by a mutex */
    if(!m_pLogStream)
    {
        /* Product a log error */
        QString cdate = QDate::currentDate().toString("dd-MM-yyyy");
        QString ctime = QTime::currentTime().toString("hh:mm");
        QString username = QString(getenv("USERNAME"));

        // try with USER environment variable
        if (username.isEmpty())
            username = QString(getenv("USER"));

        QString logName = QString("%1/%2.log").arg(m_logDirectory).arg(PACKAGE_NAME);

        m_pLogFile = new QFile(logName);

        // check the size of the file if exists => limit the log file to 10MBytes
        if (m_pLogFile->size() > 10 * 1024 * 1024)
            m_pLogFile->remove();

        if (m_pLogFile->open(QFile::WriteOnly | QFile::Append))
        {
            m_pLogStream = new QTextStream(m_pLogFile);

            *m_pLogStream << "-----------------------------------------------------" << "\n";
            *m_pLogStream << logHeaderInformation << QString(" by %1. Begin session at (%2 - %3) \n").arg(username).arg(cdate).arg(ctime);
            *m_pLogStream << "-----------------------------------------------------" << "\n\n";
        }
        else
        {
            emit sigNewMessage(QString("<font color=\"red\"><b>Can't open for writting %1</b></font> <br>").arg(logName));
        }
    }

    if(m_pLogStream)
    {
        *m_pLogStream << message << "\n";
        return true;
    }

    return false;
}
