
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

#ifndef _XSEARCHLINEEDIT_H_
#define _XSEARCHLINEEDIT_H_

#include <QtGui/QLineEdit>

class QToolButton;
class QMenu;
class QAction;

class xSearchLineEdit : public QLineEdit
{
    Q_OBJECT
public:

    xSearchLineEdit(QWidget* parent = 0);
    xSearchLineEdit(bool enableCaseSensitive, bool enableWholeWords, QWidget* parent = 0);
    virtual ~xSearchLineEdit();

    void setInactiveText(const QString& text);

protected:
    virtual void focusInEvent(QFocusEvent* event);
    virtual void paintEvent(QPaintEvent* event);
    virtual void resizeEvent(QResizeEvent* event);

signals:
	void sigCaseSensitiveToggled(bool checked);
	void sigWholeWordsToggled(bool checked);

private slots:
    void slotTextEdited(QString text);
    void slotShowOptionMenuTriggered();

private:
    void init(bool enableCaseSensitive, bool enableWholeWords);
    QMenu* createOptionMenu(bool enableCaseSensitive, bool enableWholeWords);

    QToolButton* m_pSearchButton;
    QMenu* m_pOptionMenu;
    QAction* m_pCaseSensitive;
    QAction* m_pWholeWords;
    QString m_inactiveText;
};

#endif // _SEARCHLINEEDIT_H_
