
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

#ifndef _XTHREADPOOL_H_
#define _XTHREADPOOL_H_

// this class provide a way to get access to the thread pool

#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <QtCore/QThread>
#include <QtCore/QList>

class xThreadPool : public QObject
{
    Q_OBJECT
public:
    static xThreadPool* getInstance();
    ~xThreadPool()
    {
        instanceFlag = false;
    }

    void stop();

    QThread *getThread();
    QThread *getReservedThread();

private:
	static bool instanceFlag;
	static xThreadPool *single;

	QList<QThread *> m_threadPool;
	QList<QThread *> m_reservedThreadPool;

	QMutex m_lock;

	xThreadPool(); //private constructor
};

#endif // _THREADPOOL_H_
