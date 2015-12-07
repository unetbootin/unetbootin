/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtScriptTools module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qscriptedit_p.h"
#include "qscriptsyntaxhighlighter_p.h"

#include <QtGui/qpainter.h>
#include <QtGui/qicon.h>
#include <QtGui/qboxlayout.h>
#include <QtGui/qlabel.h>
#include <QtGui/qlineedit.h>
#include <QtGui/qmenu.h>
#include <QtGui/qaction.h>
#include <QtGui/qwidgetaction.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

class QScriptEditExtraArea : public QWidget
{
public:
    QScriptEditExtraArea(QScriptEdit *edit)
        : QWidget(edit)
    {
        setMouseTracking(true);
    }

    QSize sizeHint() const {
        return QSize(editor()->extraAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event)
    {
        editor()->extraAreaPaintEvent(event);
    }
    void mousePressEvent(QMouseEvent *event)
    {
        editor()->extraAreaMouseEvent(event);
    }
    void mouseMoveEvent(QMouseEvent *event)
    {
        editor()->extraAreaMouseEvent(event);
    }
    void mouseReleaseEvent(QMouseEvent *event)
    {
        editor()->extraAreaMouseEvent(event);
    }
    bool event(QEvent *event)
    {
        if (editor()->extraAreaEvent(event))
            return true;
        return QWidget::event(event);
    }

private:
    QScriptEdit *editor() const
    {
        return qobject_cast<QScriptEdit*>(parent());
    }
};



QScriptEdit::QScriptEdit(QWidget *parent)
    : QPlainTextEdit(parent)
{
    m_baseLineNumber = 1;
    m_executionLineNumber = -1;

    m_extraArea = new QScriptEditExtraArea(this);

    QObject::connect(this, SIGNAL(blockCountChanged(int)),
                     this, SLOT(updateExtraAreaWidth()));
    QObject::connect(this, SIGNAL(updateRequest(const QRect &, int)),
                     this, SLOT(updateExtraArea(const QRect &, int)));
    QObject::connect(this, SIGNAL(cursorPositionChanged()),
                     this, SLOT(highlightCurrentLine()));

    updateExtraAreaWidth();

#ifndef QT_NO_SYNTAXHIGHLIGHTER
    (void) new QScriptSyntaxHighlighter(document());
#endif
}

QScriptEdit::~QScriptEdit()
{
}

int QScriptEdit::baseLineNumber() const
{
    return m_baseLineNumber;
}

void QScriptEdit::setBaseLineNumber(int base)
{
    m_baseLineNumber = base;
    m_extraArea->update();
}

int QScriptEdit::executionLineNumber() const
{
    return m_executionLineNumber;
}

void QScriptEdit::setExecutionLineNumber(int lineNumber, bool error)
{
    m_executionLineNumber = lineNumber;
    m_executionLineNumberHasError = error;
    m_extraArea->update();
    updateExtraSelections();
    gotoLine(lineNumber);
}

void QScriptEdit::setExecutableLineNumbers(const QSet<int> &lineNumbers)
{
    m_executableLineNumbers = lineNumbers;
}

int QScriptEdit::currentLineNumber() const
{
    return textCursor().blockNumber() + m_baseLineNumber;
}

void QScriptEdit::gotoLine(int lineNumber)
{
    int blockNumber = lineNumber - m_baseLineNumber;
    const QTextBlock &block = document()->findBlockByNumber(blockNumber);
    if (block.isValid()) {
        setTextCursor(QTextCursor(block));
        centerCursor();
    }
}

void QScriptEdit::setBreakpoint(int lineNumber)
{
    m_breakpoints[lineNumber] = BreakpointData();
    m_extraArea->update();
}

void QScriptEdit::setBreakpointEnabled(int lineNumber, bool enable)
{
    m_breakpoints[lineNumber].enabled = enable;
    m_extraArea->update();
}

void QScriptEdit::deleteBreakpoint(int lineNumber)
{
    m_breakpoints.remove(lineNumber);
    m_extraArea->update();
}

void QScriptEdit::paintEvent(QPaintEvent *e)
{
    QPlainTextEdit::paintEvent(e);
}

void QScriptEdit::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    int x = isLeftToRight() ? cr.left() : cr.left() + cr.width() - extraAreaWidth();
    m_extraArea->setGeometry(QRect(x, cr.top(), extraAreaWidth(), cr.height()));
}

void QScriptEdit::updateExtraAreaWidth()
{
    if (isLeftToRight())
        setViewportMargins(extraAreaWidth(), 0, 0, 0);
    else
        setViewportMargins(0, 0, extraAreaWidth(), 0);
}

void QScriptEdit::updateExtraArea(const QRect &rect, int dy)
{
    if (dy)
        m_extraArea->scroll(0, dy);
    else
        m_extraArea->update(0, rect.y(), m_extraArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateExtraAreaWidth();
}

void QScriptEdit::highlightCurrentLine()
{
    updateExtraSelections();
}

void QScriptEdit::updateExtraSelections()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    {
        QTextEdit::ExtraSelection selection;
        QColor lineColor = QColor(Qt::yellow).lighter(160);
        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }
    if (m_executionLineNumber != -1) {
        QTextEdit::ExtraSelection selection;
        QColor lineColor;
        if (m_executionLineNumberHasError)
            lineColor = QColor(Qt::red);
        else
            lineColor = QColor(Qt::green).lighter(160);
        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        int blockNumber = m_executionLineNumber - m_baseLineNumber;
        selection.cursor = QTextCursor(document()->findBlockByNumber(blockNumber));
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

int QScriptEdit::extraAreaWidth() const
{
    int space = 0;
    const QFontMetrics fm(fontMetrics());

    int digits = 1;
    int max = qMax(1, blockCount() + m_baseLineNumber);
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    space += fm.width(QLatin1Char('9')) * digits;

    int markWidth = fm.lineSpacing();
    space += markWidth;

    space += 4;

    return space;
}

void QScriptEdit::extraAreaPaintEvent(QPaintEvent *e)
{
    QRect rect = e->rect();
    QPalette pal = palette();
    pal.setCurrentColorGroup(QPalette::Active);
    QPainter painter(m_extraArea);
    painter.fillRect(rect, Qt::lightGray);
    const QFontMetrics fm(fontMetrics());

    int markWidth = fm.lineSpacing();
    int extraAreaWidth = m_extraArea->width();

    QLinearGradient gradient(QPointF(extraAreaWidth - 10, 0), QPointF(extraAreaWidth, 0));
    gradient.setColorAt(0, pal.color(QPalette::Background));
    gradient.setColorAt(1, pal.color(QPalette::Base));
    painter.fillRect(rect, gradient);

    QLinearGradient gradient2(QPointF(0, 0), QPointF(markWidth, 0));
    gradient2.setColorAt(0, pal.color(QPalette::Dark));
    gradient2.setColorAt(1, pal.color(QPalette::Background));
    painter.fillRect(rect.intersected(QRect(rect.x(), rect.y(), markWidth, rect.height())), gradient2);

    painter.setPen(QPen(pal.color(QPalette::Background), 2));
    if (isLeftToRight())
        painter.drawLine(rect.x() + extraAreaWidth-1, rect.top(), rect.x() + extraAreaWidth-1, rect.bottom());
    else
        painter.drawLine(rect.x(), rect.top(), rect.x(), rect.bottom());
    painter.setRenderHint(QPainter::Antialiasing);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    qreal top = blockBoundingGeometry(block).translated(contentOffset()).top();
    qreal bottom = top + blockBoundingRect(block).height();

    QString imagesPath = QString::fromLatin1(":/qt/scripttools/debugging/images");
    QString imageExt;
// SVGs don't work on all platforms, even when QT_NO_SVG is not defined, so disable SVG usage for now.
// #ifndef QT_NO_SVG
#if 0
    imageExt = QString::fromLatin1("svg");
#else
    imageExt = QString::fromLatin1("png");
#endif

    while (block.isValid() && top <= rect.bottom()) {
        if (block.isVisible() && bottom >= rect.top()) {

            int lineNumber = blockNumber + m_baseLineNumber;
            if (m_breakpoints.contains(lineNumber)) {
                int radius = fm.lineSpacing() - 1;
                QRect r(rect.x(), (int)top, radius, radius);
                QIcon icon(m_breakpoints[lineNumber].enabled
                           ? QString::fromLatin1("%0/breakpoint.%1").arg(imagesPath).arg(imageExt)
                           : QString::fromLatin1("%0/d_breakpoint.%1").arg(imagesPath).arg(imageExt));
                icon.paint(&painter, r, Qt::AlignCenter);
            }
            if (m_executionLineNumber == lineNumber) {
                int radius = fm.lineSpacing() - 1;
                QRect r(rect.x(), (int)top, radius, radius);
                QIcon icon(QString::fromLatin1("%0/location.%1").arg(imagesPath).arg(imageExt));
                icon.paint(&painter, r, Qt::AlignCenter);
            }

#if QT_VERSION >= 0x040500
            if (!m_executableLineNumbers.contains(lineNumber))
                painter.setPen(pal.color(QPalette::Mid));
            else
#endif
                painter.setPen(QColor(Qt::darkCyan));
            QString number = QString::number(lineNumber);
            painter.drawText(rect.x() + markWidth, (int)top, rect.x() + extraAreaWidth - markWidth - 4,
                             fm.height(), Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + blockBoundingRect(block).height();
        ++blockNumber;
    }
}

void QScriptEdit::extraAreaMouseEvent(QMouseEvent *e)
{
    QTextCursor cursor = cursorForPosition(QPoint(0, e->pos().y()));
    cursor.setPosition(cursor.block().position());

    QFontMetrics fm(font());
    int markWidth = fm.lineSpacing();

    if (e->type() == QEvent::MouseMove && e->buttons() == 0) { // mouse tracking
        bool hand = (e->pos().x() <= markWidth);
#if QT_VERSION >= 0x040500
        int lineNumber = cursor.blockNumber() + m_baseLineNumber;
        hand = hand && m_executableLineNumbers.contains(lineNumber);
#endif
#ifndef QT_NO_CURSOR
        if (hand != (m_extraArea->cursor().shape() == Qt::PointingHandCursor))
            m_extraArea->setCursor(hand ? Qt::PointingHandCursor : Qt::ArrowCursor);
#endif
    }

    if (e->type() == QEvent::MouseButtonPress) {
        if (e->button() == Qt::LeftButton) {
#if QT_VERSION >= 0x040500
            int lineNumber = cursor.blockNumber() + m_baseLineNumber;
            bool executable = m_executableLineNumbers.contains(lineNumber);
#else
            bool executable = true;
#endif
            if ((e->pos().x() <= markWidth) && executable)
                m_extraAreaToggleBlockNumber = cursor.blockNumber();
            else
                m_extraAreaToggleBlockNumber = -1;
        }
    } else if (e->type() == QEvent::MouseButtonRelease) {
        if (e->button() == Qt::LeftButton) {
            if ((m_extraAreaToggleBlockNumber != -1) && (e->pos().x() <= markWidth)) {
                int lineNumber = m_extraAreaToggleBlockNumber + m_baseLineNumber;
                bool on = !m_breakpoints.contains(lineNumber);
                emit breakpointToggleRequest(lineNumber, on);
            }
        } else if (e->button() == Qt::RightButton) {
            int lineNumber = cursor.blockNumber() + m_baseLineNumber;
#if QT_VERSION >= 0x040500
            if (!m_executableLineNumbers.contains(lineNumber))
                return;
#endif
            bool has = m_breakpoints.contains(lineNumber);
            QMenu *popup = new QMenu();
            QAction *toggleAct = new QAction(QObject::tr("Toggle Breakpoint"), popup);
            popup->addAction(toggleAct);
            QAction *disableAct = new QAction(QObject::tr("Disable Breakpoint"), popup);
            QAction *enableAct = new QAction(QObject::tr("Enable Breakpoint"), popup);
            QWidget *conditionWidget = new QWidget();
            {
                QHBoxLayout *hbox = new QHBoxLayout(conditionWidget);
                hbox->addWidget(new QLabel(QObject::tr("Breakpoint Condition:")));
                hbox->addWidget(new QLineEdit());
            }
//            QWidgetAction *conditionAct = new QWidgetAction(popup);
//            conditionAct->setDefaultWidget(conditionWidget);
            if (has) {
                popup->addSeparator();
                popup->addAction(m_breakpoints[lineNumber].enabled ? disableAct : enableAct);
//                popup->addAction(conditionAct);
            }
            QAction *ret = popup->exec(e->globalPos());
            if (ret) {
                if (ret == toggleAct) {
                    emit breakpointToggleRequest(lineNumber, !has);
                } else if (ret == disableAct) {
                    emit breakpointEnableRequest(lineNumber, false);
                } else if (ret == enableAct) {
                    emit breakpointEnableRequest(lineNumber, true);
                }// else if (ret == conditionAct) {
                //}
            }
            popup->deleteLater();
        }
    }
}

bool QScriptEdit::extraAreaEvent(QEvent *e)
{
    if (e->type() == QEvent::ToolTip) {
        // ### show the breakpoint's condition, if any
        return true;
    }
    return false;
}

QT_END_NAMESPACE
