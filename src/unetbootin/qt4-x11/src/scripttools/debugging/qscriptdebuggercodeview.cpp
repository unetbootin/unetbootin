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

#include "qscriptdebuggercodeview_p.h"
#include "qscriptdebuggercodeviewinterface_p_p.h"

#include "qscriptedit_p.h"

#include <QtGui/qboxlayout.h>
#include <QtGui/qtextobject.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

class QScriptDebuggerCodeViewPrivate
    : public QScriptDebuggerCodeViewInterfacePrivate
{
    Q_DECLARE_PUBLIC(QScriptDebuggerCodeView)
public:
    QScriptDebuggerCodeViewPrivate();
    ~QScriptDebuggerCodeViewPrivate();

    QScriptEdit *editor;
};

QScriptDebuggerCodeViewPrivate::QScriptDebuggerCodeViewPrivate()
{
}

QScriptDebuggerCodeViewPrivate::~QScriptDebuggerCodeViewPrivate()
{
}

QScriptDebuggerCodeView::QScriptDebuggerCodeView(QWidget *parent)
    : QScriptDebuggerCodeViewInterface(*new QScriptDebuggerCodeViewPrivate, parent, 0)
{
    Q_D(QScriptDebuggerCodeView);
    d->editor = new QScriptEdit();
    d->editor->setReadOnly(true);
    d->editor->setBackgroundVisible(false);
    QObject::connect(d->editor, SIGNAL(breakpointToggleRequest(int,bool)),
                     this, SIGNAL(breakpointToggleRequest(int,bool)));
    QObject::connect(d->editor, SIGNAL(breakpointEnableRequest(int,bool)),
                     this, SIGNAL(breakpointEnableRequest(int,bool)));
    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->addWidget(d->editor);
}

QScriptDebuggerCodeView::~QScriptDebuggerCodeView()
{
}

QString QScriptDebuggerCodeView::text() const
{
    Q_D(const QScriptDebuggerCodeView);
    return d->editor->toPlainText();
}

void QScriptDebuggerCodeView::setText(const QString &text)
{
    Q_D(QScriptDebuggerCodeView);
    d->editor->setPlainText(text);
}

int QScriptDebuggerCodeView::cursorLineNumber() const
{
    Q_D(const QScriptDebuggerCodeView);
    return d->editor->currentLineNumber();
}

void QScriptDebuggerCodeView::gotoLine(int lineNumber)
{
    Q_D(QScriptDebuggerCodeView);
    d->editor->gotoLine(lineNumber);
}

int QScriptDebuggerCodeView::find(const QString &exp, int options)
{
    Q_D(QScriptDebuggerCodeView);
    QPlainTextEdit *ed = (QPlainTextEdit*)d->editor;
    QTextCursor cursor = ed->textCursor();
    if (options & 0x100) {
        // start searching from the beginning of selection
        if (cursor.hasSelection()) {
            int len = cursor.selectedText().length();
            cursor.clearSelection();
            cursor.setPosition(cursor.position() - len);
            ed->setTextCursor(cursor);
        }
        options &= ~0x100;
    }
    int ret = 0;
    if (ed->find(exp, QTextDocument::FindFlags(options))) {
        ret |= 0x1;
    } else {
        QTextCursor curse = cursor;
        curse.movePosition(QTextCursor::Start);
        ed->setTextCursor(curse);
        if (ed->find(exp, QTextDocument::FindFlags(options)))
            ret |= 0x1 | 0x2;
        else
            ed->setTextCursor(cursor);
    }
    return ret;
}

void QScriptDebuggerCodeView::setExecutionLineNumber(int lineNumber, bool error)
{
    Q_D(QScriptDebuggerCodeView);
    d->editor->setExecutionLineNumber(lineNumber, error);
}

void QScriptDebuggerCodeView::setExecutableLineNumbers(const QSet<int> &lineNumbers)
{
    Q_D(QScriptDebuggerCodeView);
    d->editor->setExecutableLineNumbers(lineNumbers);
}

int QScriptDebuggerCodeView::baseLineNumber() const
{
    Q_D(const QScriptDebuggerCodeView);
    return d->editor->baseLineNumber();
}

void QScriptDebuggerCodeView::setBaseLineNumber(int lineNumber)
{
    Q_D(QScriptDebuggerCodeView);
    d->editor->setBaseLineNumber(lineNumber);
}

void QScriptDebuggerCodeView::setBreakpoint(int lineNumber)
{
    Q_D(QScriptDebuggerCodeView);
    d->editor->setBreakpoint(lineNumber);
}

void QScriptDebuggerCodeView::deleteBreakpoint(int lineNumber)
{
    Q_D(QScriptDebuggerCodeView);
    d->editor->deleteBreakpoint(lineNumber);
}

void QScriptDebuggerCodeView::setBreakpointEnabled(int lineNumber, bool enable)
{
    Q_D(QScriptDebuggerCodeView);
    d->editor->setBreakpointEnabled(lineNumber, enable);
}

namespace {

static bool isIdentChar(const QChar &ch)
{
    static QChar underscore = QLatin1Char('_');
    return ch.isLetter() || (ch == underscore);
}

} // namespace

/*!
  \reimp
*/
bool QScriptDebuggerCodeView::event(QEvent *e)
{
    Q_D(QScriptDebuggerCodeView);
    if (e->type() == QEvent::ToolTip) {
        if (d->editor->executionLineNumber() == -1)
            return false;
        QHelpEvent *he = static_cast<QHelpEvent*>(e);
        QPoint pt = he->pos();
        pt.rx() -= d->editor->extraAreaWidth();
        pt.ry() -= 8;
        QTextCursor cursor = d->editor->cursorForPosition(pt);
        QTextBlock block = cursor.block();
        QString contents = block.text();
        if (contents.isEmpty())
            return false;
        int linePosition = cursor.position() - block.position();
        linePosition -= 3;
        if (linePosition < 0)
            linePosition = 0;

        // ### generalize -- same as in completiontask

        int pos = linePosition;
        if ((pos > 0) && contents.at(pos-1).isNumber()) {
            // tooltips for numbers is pointless
            return false;
        }

        while ((pos > 0) && isIdentChar(contents.at(pos-1)))
            --pos;
        if ((pos > 0) && ((contents.at(pos-1) == QLatin1Char('\''))
                          || (contents.at(pos-1) == QLatin1Char('\"')))) {
            // ignore string literals
            return false;
        }
        int pos2 = linePosition;
        while ((pos2 < contents.size()-1) && isIdentChar(contents.at(pos2+1)))
            ++pos2;
        QString ident = contents.mid(pos, pos2 - pos + 1);

        QStringList path;
        path.append(ident);
        while ((pos > 0) && (contents.at(pos-1) == QLatin1Char('.'))) {
            --pos;
            pos2 = pos;
            while ((pos > 0) && isIdentChar(contents.at(pos-1)))
                --pos;
            path.prepend(contents.mid(pos, pos2 - pos));
        }

        if (!path.isEmpty()) {
            int lineNumber = cursor.blockNumber() + d->editor->baseLineNumber();
            emit toolTipRequest(he->globalPos(), lineNumber, path);
        }
    }
    return false;
}

QT_END_NAMESPACE

#include "moc_qscriptdebuggercodeview_p.cpp"
