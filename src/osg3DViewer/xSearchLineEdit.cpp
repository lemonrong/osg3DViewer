
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

#include "xSearchLineEdit.h"

#include <QtGui/QToolButton>
#include <QtGui/QMenu>
#include <QtGui/QAction>
#include <QtGui/QPainter>

xSearchLineEdit::xSearchLineEdit(QWidget* parent)
    : QLineEdit(parent)
    , m_pSearchButton(new QToolButton(this))
    , m_pOptionMenu(0)
    , m_pCaseSensitive(0)
    , m_pWholeWords(0)
{
    init(false, false);
}

xSearchLineEdit::xSearchLineEdit(bool enableCaseSensitive, bool enableWholeWords, QWidget* parent)
    : QLineEdit(parent)
    , m_pSearchButton(new QToolButton(this))
    , m_pOptionMenu(0)
    , m_pCaseSensitive(0)
    , m_pWholeWords(0)
{
    init(enableCaseSensitive, enableWholeWords);
}

xSearchLineEdit::~xSearchLineEdit()
{
    delete m_pCaseSensitive;
    delete m_pWholeWords;
    delete m_pOptionMenu;
    delete m_pSearchButton;
}

void xSearchLineEdit::init(bool enableCaseSensitive, bool enableWholeWords)
{
    m_pOptionMenu = createOptionMenu(enableCaseSensitive, enableWholeWords);

    QPixmap pixmap(":/osg3DViewer/mag.png");
    m_pSearchButton->setIcon(QIcon(pixmap));
    m_pSearchButton->setIconSize(pixmap.size());
    m_pSearchButton->setCursor(Qt::ArrowCursor);
#if defined(Q_OS_LINUX)
    m_pSearchButton->setStyleSheet("QToolButton { border: none; padding: 0px; margin-left: 3px; }");
    setTextMargins(pixmap.width() + 6, 0, 0, 0);
#elif defined(Q_OS_WIN32)
    m_pSearchButton->setStyleSheet("QToolButton { border: none; padding: 0px; margin-left: 1px; }");
    setTextMargins(pixmap.width() + 3, 0, 0, 0);
#endif
    setInactiveText(tr("<type text>"));

    connect(m_pSearchButton, SIGNAL(clicked()), this, SLOT(slotShowOptionMenuTriggered()));
    connect(this, SIGNAL(textEdited(QString)), this, SLOT(slotTextEdited(QString)));
}

QMenu* xSearchLineEdit::createOptionMenu(bool enableCaseSensitive, bool enableWholeWords)
{
    QMenu* optionMenu = new QMenu(this);

    m_pCaseSensitive = new QAction(tr("Case Sensitive"), this);
    m_pCaseSensitive->setCheckable(true);
    m_pCaseSensitive->setChecked(enableCaseSensitive);
    connect(m_pCaseSensitive, SIGNAL(toggled(bool)), this, SIGNAL(sigCaseSensitiveToggled(bool)));
    optionMenu->addAction(m_pCaseSensitive);
    return optionMenu;
}

void xSearchLineEdit::setInactiveText(const QString& text)
{
    m_inactiveText = text;
}

void xSearchLineEdit::focusInEvent(QFocusEvent* event)
{
    QLineEdit::focusInEvent(event);

    if (displayText().length() > 0)
        selectAll();
}

void xSearchLineEdit::resizeEvent(QResizeEvent* event)
{
    QLineEdit::resizeEvent(event);

    int y = (size().height() - m_pSearchButton->iconSize().height()) / 2 - 1;

    if (y > 0)
        m_pSearchButton->move(m_pSearchButton->x(), y);
}

void xSearchLineEdit::paintEvent(QPaintEvent* event)
{
    QLineEdit::paintEvent(event);

    if (text().isEmpty() && !m_inactiveText.isEmpty() && !hasFocus())
    {
        QPainter painter(this);
        painter.setPen(QColor(Qt::lightGray));

        int left, top, right, bottom;
        getTextMargins(&left, &top, &right, &bottom);
        painter.drawText(left, top, width(), height(), Qt::AlignLeft | Qt::AlignVCenter, m_inactiveText);
    }
}

void xSearchLineEdit::slotTextEdited(QString text)
{
    if (text.contains("\n"))
    {
        int index = text.indexOf("\n");
        text.remove(index, text.length());
        setText(text);
    }
}

void xSearchLineEdit::slotShowOptionMenuTriggered()
{
    Q_ASSERT(m_pOptionMenu);

    m_pOptionMenu->exec(QCursor::pos());
}
