
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

#ifndef _XOSGLLOGER_H_
#define _XOSGLLOGER_H_

#include <iostream>
#include <streambuf>
#include <string>
#include <QtCore/QObject>
#include <QtCore/QString>

class xOsgLogger : public QObject, public std::basic_streambuf<char>
{
    Q_OBJECT

public:
    xOsgLogger(std::ostream &stream) : m_stream(stream)
    {
        m_pOld_buf = stream.rdbuf();
        stream.rdbuf(this);
    }
    ~xOsgLogger()
    {
        // output anything that is left
        if (!m_string.empty())
            emit sigMessage(QString(m_string.c_str()));

        m_stream.rdbuf(m_pOld_buf);
    }

signals:
    void sigMessage(const QString &);

protected:
    virtual int_type overflow(int_type v)
    {
        if (v == '\n')
        {
            emit sigMessage(QString(m_string.c_str()));
            m_string.erase(m_string.begin(), m_string.end());
        }
        else
            m_string += v;

        return v;
    }

    virtual std::streamsize xsputn(const char *p, std::streamsize n)
    {
        m_string.append(p, p + n);

        size_t pos = 0;
        while (pos != std::string::npos)
        {
            pos = m_string.find('\n');
            if (pos != std::string::npos)
            {
                std::string tmp(m_string.begin(), m_string.begin() + pos);
                emit sigMessage(QString(tmp.c_str()));
                m_string.erase(m_string.begin(), m_string.begin() + pos + 1);
            }
        }

        return n;
    }

private:

    std::ostream &m_stream;
    std::streambuf *m_pOld_buf;
    std::string m_string;
};

#endif // _OSGLLOGER_H_
