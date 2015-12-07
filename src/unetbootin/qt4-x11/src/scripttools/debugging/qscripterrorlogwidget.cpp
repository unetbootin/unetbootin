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

#include "qscripterrorlogwidget_p.h"
#include "qscripterrorlogwidgetinterface_p_p.h"

#include <QtCore/qdatetime.h>
#include <QtGui/qboxlayout.h>
#include <QtGui/qtextedit.h>
#include <QtGui/qscrollbar.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

namespace {

class OutputEdit : public QTextEdit
{
public:
    OutputEdit(QWidget *parent = 0)
        : QTextEdit(parent)
    {
        setReadOnly(true);
//        setFocusPolicy(Qt::NoFocus);
        document()->setMaximumBlockCount(255);
    }

    void scrollToBottom()
    {
        QScrollBar *bar = verticalScrollBar();
        bar->setValue(bar->maximum());
    }
};

} // namespace

class QScriptErrorLogWidgetPrivate
    : public QScriptErrorLogWidgetInterfacePrivate
{
    Q_DECLARE_PUBLIC(QScriptErrorLogWidget)
public:
    QScriptErrorLogWidgetPrivate();
    ~QScriptErrorLogWidgetPrivate();

    OutputEdit *outputEdit;
};

QScriptErrorLogWidgetPrivate::QScriptErrorLogWidgetPrivate()
{
}

QScriptErrorLogWidgetPrivate::~QScriptErrorLogWidgetPrivate()
{
}

QScriptErrorLogWidget::QScriptErrorLogWidget(QWidget *parent)
    : QScriptErrorLogWidgetInterface(*new QScriptErrorLogWidgetPrivate, parent, 0)
{
    Q_D(QScriptErrorLogWidget);
    d->outputEdit = new OutputEdit();
    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->setMargin(0);
    vbox->setSpacing(0);
    vbox->addWidget(d->outputEdit);

//    QString sheet = QString::fromLatin1("font-size: 14px; font-family: \"Monospace\";");
//    setStyleSheet(sheet);
}

QScriptErrorLogWidget::~QScriptErrorLogWidget()
{
}

void QScriptErrorLogWidget::message(
    QtMsgType type, const QString &text, const QString &fileName,
    int lineNumber, int columnNumber, const QVariant &/*data*/)
{
    // ### we need the error message rather than Error.toString()
    Q_UNUSED(type);
    Q_UNUSED(fileName);
    Q_UNUSED(lineNumber);
    Q_UNUSED(columnNumber);
    Q_D(QScriptErrorLogWidget);
    QString html;
    html.append(QString::fromLatin1("<b>%0</b> %1<br>")
                .arg(QDateTime::currentDateTime().toString()).arg(Qt::escape(text)));
    d->outputEdit->insertHtml(html);
    d->outputEdit->scrollToBottom();
}

void QScriptErrorLogWidget::clear()
{
    Q_D(QScriptErrorLogWidget);
    d->outputEdit->clear();
}

QT_END_NAMESPACE
