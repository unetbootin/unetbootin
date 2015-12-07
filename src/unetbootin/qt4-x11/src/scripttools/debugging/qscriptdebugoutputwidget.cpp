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

#include "qscriptdebugoutputwidget_p.h"
#include "qscriptdebugoutputwidgetinterface_p_p.h"

#include <QtCore/qdebug.h>
#include <QtGui/qboxlayout.h>
#include <QtGui/qplaintextedit.h>
#include <QtGui/qscrollbar.h>

QT_BEGIN_NAMESPACE

namespace {

class OutputEdit : public QPlainTextEdit
{
public:
    OutputEdit(QWidget *parent = 0)
        : QPlainTextEdit(parent)
    {
        setReadOnly(true);
//        setFocusPolicy(Qt::NoFocus);
        setMaximumBlockCount(2500);
    }

    void scrollToBottom()
    {
        QScrollBar *bar = verticalScrollBar();
        bar->setValue(bar->maximum());
    }

    int charactersPerLine() const
    {
        QFontMetrics fm(font());
        return width() / fm.maxWidth();
    }
};

} // namespace

class QScriptDebugOutputWidgetPrivate
    : public QScriptDebugOutputWidgetInterfacePrivate
{
    Q_DECLARE_PUBLIC(QScriptDebugOutputWidget)
public:
    QScriptDebugOutputWidgetPrivate();
    ~QScriptDebugOutputWidgetPrivate();

    OutputEdit *outputEdit;
};

QScriptDebugOutputWidgetPrivate::QScriptDebugOutputWidgetPrivate()
{
}

QScriptDebugOutputWidgetPrivate::~QScriptDebugOutputWidgetPrivate()
{
}

QScriptDebugOutputWidget::QScriptDebugOutputWidget(QWidget *parent)
    : QScriptDebugOutputWidgetInterface(*new QScriptDebugOutputWidgetPrivate, parent, 0)
{
    Q_D(QScriptDebugOutputWidget);
    d->outputEdit = new OutputEdit();
    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->setMargin(0);
    vbox->setSpacing(0);
    vbox->addWidget(d->outputEdit);

#ifndef QT_NO_STYLE_STYLESHEET
    QString sheet = QString::fromLatin1("font-size: 14px; font-family: \"Monospace\";");
    setStyleSheet(sheet);
#endif
}

QScriptDebugOutputWidget::~QScriptDebugOutputWidget()
{
}

void QScriptDebugOutputWidget::message(
    QtMsgType type, const QString &text, const QString &fileName,
    int lineNumber, int columnNumber, const QVariant &/*data*/)
{
    // ### unify with QScriptDebuggerConsoleWidget::message()
    Q_D(QScriptDebugOutputWidget);
    QString msg;
    if (!fileName.isEmpty() || (lineNumber != -1)) {
        if (!fileName.isEmpty())
            msg.append(fileName);
        else
            msg.append(QLatin1String("<noname>"));
        if (lineNumber != -1) {
            msg.append(QLatin1Char(':'));
            msg.append(QString::number(lineNumber));
            if (columnNumber != -1) {
                msg.append(QLatin1Char(':'));
                msg.append(QString::number(columnNumber));
            }
        }
        msg.append(QLatin1String(": "));
    }
    msg.append(text);
    QTextCharFormat oldFmt = d->outputEdit->currentCharFormat();
    QTextCharFormat fmt(oldFmt);
    if (type == QtCriticalMsg) {
        fmt.setForeground(Qt::red);
        d->outputEdit->setCurrentCharFormat(fmt);
    }
    d->outputEdit->appendPlainText(msg);
    d->outputEdit->setCurrentCharFormat(oldFmt);
    d->outputEdit->scrollToBottom();
}

void QScriptDebugOutputWidget::clear()
{
    Q_D(QScriptDebugOutputWidget);
    d->outputEdit->clear();
}

QT_END_NAMESPACE
