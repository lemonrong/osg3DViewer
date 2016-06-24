
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

#ifndef _XLOGHANDLER_H_
#define _XLOGHANDLER_H_

#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <QtCore/QStringList>

class QTimer;
class QTextStream;
class QFile;

/*!
 *	get log messages and display a colored and timed message by taking care of the message level
 */

class xLogHandler : public QObject
{
    Q_OBJECT

public:
    enum LogLevel
    {
        LOG_DEBUG = 0,                      /*!< Message only for debugging purpose. */
        LOG_INFO,                           /*!< Information message. */
        LOG_WARNING,                        /*!< Warning, abnormal event. */
        LOG_ERROR                           /*!< Error, invalid file. */
    };

    static xLogHandler *getInstance();
    ~xLogHandler();

    void setMessageLevel(LogLevel level)
    {
        m_currLevel = level;
    }

    void setLogDirectory(const QString &);
    void setLogToFile(bool);

    void setBufferization(bool);
    void startEmission(bool);

public slots:
    void slotReportDebug(const QString &message);
    void slotReportInfo(const QString &message);
    void slotReportWarning(const QString &message);
    void slotReportError(const QString &message);

signals:
    void sigNewMessage(const QString &);
    void sigNewMessages(const QStringList &);

private slots:
    void slotUnqueueWaitingMessages();

private:
    xLogHandler();

    xLogHandler(const xLogHandler &); // hide copy constructor
    xLogHandler& operator=(const xLogHandler &); // hide assign op
    // we leave just the declarations, so the compiler will warn us
    // if we try to use those two functions by accident

    void reportMessage(LogLevel level, const QString &message);
    void unqueueMessages();
    void loadSettings();
    bool fillAppLogFile(const QString &message);

    QMutex m_lock;
    static xLogHandler* m_pInstance;
    LogLevel m_currLevel;
    QStringList m_buffer;
    bool m_bBufferized;
    bool m_bStartEmission;
    QTimer *m_pTimer;
    QString m_logDirectory;
    QTextStream * m_pLogStream;
    QFile *m_pLogFile;
    bool m_bSaveToLog;
};

#endif  // _LOGHANDLER_H_
