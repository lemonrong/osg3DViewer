
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

#include "xThreadPool.h"

bool xThreadPool::instanceFlag = false;
xThreadPool* xThreadPool::single = NULL;

xThreadPool* xThreadPool::getInstance()
{
    if(!instanceFlag)
    {
        single = new xThreadPool();
        instanceFlag = true;
        return single;
    }
    else
    {
        return single;
    }
}

xThreadPool::xThreadPool()
{
// create the treads
    for (int i = 0; i <  QThread::idealThreadCount(); i++)
    {
        m_threadPool << new QThread;
        qDebug("ThreadPool: start thread %d ",i);
        m_threadPool.at(i)->start();
    }
}

void xThreadPool::stop()
{
    // stop each running thread properly
    foreach(QThread * th, m_threadPool)
    {
        if (th->isRunning())
        {
            th->quit();
            th->wait(1000);
        }
    }

    foreach(QThread * th, m_reservedThreadPool)
    {
        if (th->isRunning())
        {
            th->quit();
            th->wait(1000);
        }
    }
}

QThread *xThreadPool::getThread()
{
    int index = qrand() % m_threadPool.size();
    qDebug("send thread %d to caller",index);
    return m_threadPool[qrand() % m_threadPool.size()];
}

QThread *xThreadPool::getReservedThread()
{
    qDebug("send a reserved thread to caller");
    QThread *th = new QThread();
    m_reservedThreadPool << th;
    th->start();
    return th;
}
