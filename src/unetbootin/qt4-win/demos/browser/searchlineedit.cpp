/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "searchlineedit.h"

#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>
#include <QtGui/QMenu>
#include <QtGui/QStyle>
#include <QtGui/QStyleOptionFrameV2>

ClearButton::ClearButton(QWidget *parent)
  : QAbstractButton(parent)
{
    setCursor(Qt::ArrowCursor);
    setToolTip(tr("Clear"));
    setVisible(false);
    setFocusPolicy(Qt::NoFocus);
}

void ClearButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    int height = this->height();

    painter.setRenderHint(QPainter::Antialiasing, true);
    QColor color = palette().color(QPalette::Mid);
    painter.setBrush(isDown()
                     ? palette().color(QPalette::Dark)
                     : palette().color(QPalette::Mid));
    painter.setPen(painter.brush().color());
    int size = width();
    int offset = size / 5;
    int radius = size - offset * 2;
    painter.drawEllipse(offset, offset, radius, radius);

    painter.setPen(palette().color(QPalette::Base));
    int border = offset * 2;
    painter.drawLine(border, border, width() - border, height - border);
    painter.drawLine(border, height - border, width() - border, border);
}

void ClearButton::textChanged(const QString &text)
{
    setVisible(!text.isEmpty());
}

/*
    Search icon on the left hand side of the search widget
    When a menu is set a down arrow appears
 */
class SearchButton : public QAbstractButton {
public:
    SearchButton(QWidget *parent = 0);
    void paintEvent(QPaintEvent *event);
    QMenu *m_menu;

protected:
    void mousePressEvent(QMouseEvent *event);
};

SearchButton::SearchButton(QWidget *parent)
  : QAbstractButton(parent),
    m_menu(0)
{
    setObjectName(QLatin1String("SearchButton"));
    setCursor(Qt::ArrowCursor);
    setFocusPolicy(Qt::NoFocus);
}

void SearchButton::mousePressEvent(QMouseEvent *event)
{
    if (m_menu && event->button() == Qt::LeftButton) {
        QWidget *p = parentWidget();
        if (p) {
            QPoint r = p->mapToGlobal(QPoint(0, p->height()));
            m_menu->exec(QPoint(r.x() + height() / 2, r.y()));
        }
        event->accept();
    }
    QAbstractButton::mousePressEvent(event);
}

void SearchButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainterPath myPath;

    int radius = (height() / 5) * 2;
    QRect circle(height() / 3 - 1, height() / 4, radius, radius);
    myPath.addEllipse(circle);

    myPath.arcMoveTo(circle, 300);
    QPointF c = myPath.currentPosition();
    int diff = height() / 7;
    myPath.lineTo(qMin(width() - 2, (int)c.x() + diff), c.y() + diff);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(Qt::darkGray, 2));
    painter.drawPath(myPath);

    if (m_menu) {
        QPainterPath dropPath;
        dropPath.arcMoveTo(circle, 320);
        QPointF c = dropPath.currentPosition();
        c = QPointF(c.x() + 3.5, c.y() + 0.5);
        dropPath.moveTo(c);
        dropPath.lineTo(c.x() + 4, c.y());
        dropPath.lineTo(c.x() + 2, c.y() + 2);
        dropPath.closeSubpath();
        painter.setPen(Qt::darkGray);
        painter.setBrush(Qt::darkGray);
        painter.setRenderHint(QPainter::Antialiasing, false);
        painter.drawPath(dropPath);
    }
    painter.end();
}

/*
    SearchLineEdit is an enhanced QLineEdit
    - A Search icon on the left with optional menu
    - When there is no text and doesn't have focus an "inactive text" is displayed
    - When there is text a clear button is displayed on the right hand side
 */
SearchLineEdit::SearchLineEdit(QWidget *parent) : ExLineEdit(parent),
    m_searchButton(new SearchButton(this))
{
    setLeftWidget(m_searchButton);
    m_inactiveText = tr("Search");

    QSizePolicy policy = sizePolicy();
    setSizePolicy(QSizePolicy::Preferred, policy.verticalPolicy());
}

void SearchLineEdit::paintEvent(QPaintEvent *event)
{
    if (lineEdit()->text().isEmpty() && !hasFocus() && !m_inactiveText.isEmpty()) {
        ExLineEdit::paintEvent(event);
        QStyleOptionFrameV2 panel;
        initStyleOption(&panel);
        QRect r = style()->subElementRect(QStyle::SE_LineEditContents, &panel, this);
        QFontMetrics fm = fontMetrics();
        int horizontalMargin = lineEdit()->x();
        QRect lineRect(horizontalMargin + r.x(), r.y() + (r.height() - fm.height() + 1) / 2,
                       r.width() - 2 * horizontalMargin, fm.height());
        QPainter painter(this);
        painter.setPen(palette().brush(QPalette::Disabled, QPalette::Text).color());
        painter.drawText(lineRect, Qt::AlignLeft|Qt::AlignVCenter, m_inactiveText);
    } else {
        ExLineEdit::paintEvent(event);
    }
}

void SearchLineEdit::resizeEvent(QResizeEvent *event)
{
    updateGeometries();
    ExLineEdit::resizeEvent(event);
}

void SearchLineEdit::updateGeometries()
{
    int menuHeight = height();
    int menuWidth = menuHeight + 1;
    if (!m_searchButton->m_menu)
        menuWidth = (menuHeight / 5) * 4;
    m_searchButton->resize(QSize(menuWidth, menuHeight));
}

QString SearchLineEdit::inactiveText() const
{
    return m_inactiveText;
}

void SearchLineEdit::setInactiveText(const QString &text)
{
    m_inactiveText = text;
}

void SearchLineEdit::setMenu(QMenu *menu)
{
    if (m_searchButton->m_menu)
        m_searchButton->m_menu->deleteLater();
    m_searchButton->m_menu = menu;
    updateGeometries();
}

QMenu *SearchLineEdit::menu() const
{
    if (!m_searchButton->m_menu) {
        m_searchButton->m_menu = new QMenu(m_searchButton);
        if (isVisible())
            (const_cast<SearchLineEdit*>(this))->updateGeometries();
    }
    return m_searchButton->m_menu;
}

