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

#include "qscriptdebuggercodefinderwidget_p.h"
#include "qscriptdebuggercodefinderwidgetinterface_p_p.h"

#include <QtGui/qboxlayout.h>
#include <QtGui/qlineedit.h>
#include <QtGui/qcheckbox.h>
#include <QtGui/qlabel.h>
#include <QtGui/qtoolbutton.h>
#include <QtGui/qevent.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

class QScriptDebuggerCodeFinderWidgetPrivate
    : public QScriptDebuggerCodeFinderWidgetInterfacePrivate
{
    Q_DECLARE_PUBLIC(QScriptDebuggerCodeFinderWidget)
public:
    QScriptDebuggerCodeFinderWidgetPrivate();
    ~QScriptDebuggerCodeFinderWidgetPrivate();

    // private slots
    void _q_updateButtons();
    void _q_onTextChanged(const QString &);
    void _q_next();
    void _q_previous();

    int findOptions() const;

    QLineEdit *editFind;
    QCheckBox *checkCase;
    QLabel *labelWrapped;
    QToolButton *toolNext;
    QToolButton *toolClose;
    QToolButton *toolPrevious;
    QCheckBox *checkWholeWords;
};

QScriptDebuggerCodeFinderWidgetPrivate::QScriptDebuggerCodeFinderWidgetPrivate()
{
}

QScriptDebuggerCodeFinderWidgetPrivate::~QScriptDebuggerCodeFinderWidgetPrivate()
{
}

void QScriptDebuggerCodeFinderWidgetPrivate::_q_updateButtons()
{
    if (editFind->text().isEmpty()) {
        toolPrevious->setEnabled(false);
        toolNext->setEnabled(false);
    } else {
        toolPrevious->setEnabled(true);
        toolNext->setEnabled(true);
    }
}

int QScriptDebuggerCodeFinderWidgetPrivate::findOptions() const
{
    int flags = 0;
    if (checkCase->isChecked())
        flags |= QTextDocument::FindCaseSensitively;
    if (checkWholeWords->isChecked())
        flags |= QTextDocument::FindWholeWords;
    return flags;
}

void QScriptDebuggerCodeFinderWidgetPrivate::_q_onTextChanged(const QString &text)
{
    emit q_func()->findRequest(text, findOptions() | 0x100);
}

void QScriptDebuggerCodeFinderWidgetPrivate::_q_next()
{
    emit q_func()->findRequest(editFind->text(), findOptions());
}

void QScriptDebuggerCodeFinderWidgetPrivate::_q_previous()
{
    emit q_func()->findRequest(editFind->text(), findOptions() | QTextDocument::FindBackward);
}

QScriptDebuggerCodeFinderWidget::QScriptDebuggerCodeFinderWidget(QWidget *parent)
    : QScriptDebuggerCodeFinderWidgetInterface(
        *new QScriptDebuggerCodeFinderWidgetPrivate, parent, 0)
{
    Q_D(QScriptDebuggerCodeFinderWidget);
    QString system = QLatin1String("win");
    QHBoxLayout *hboxLayout = new QHBoxLayout(this);
#ifdef Q_OS_MAC
    system = QLatin1String("mac");
#else
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
#endif

    d->toolClose = new QToolButton(this);
    d->toolClose->setIcon(QIcon(QString::fromUtf8(":/qt/scripttools/debugging/images/%1/closetab.png").arg(system)));
    d->toolClose->setAutoRaise(true);
    d->toolClose->setText(QObject::tr("Close"));
    hboxLayout->addWidget(d->toolClose);

    d->editFind = new QLineEdit(this);
    d->editFind->setMinimumSize(QSize(150, 0));
    connect(d->editFind, SIGNAL(textChanged(const QString&)),
            this, SLOT(_q_updateButtons()));
    connect(d->editFind, SIGNAL(returnPressed()),
            this, SLOT(_q_next()));
    hboxLayout->addWidget(d->editFind);

    d->toolPrevious = new QToolButton(this);
    d->toolPrevious->setAutoRaise(true);
    d->toolPrevious->setText(tr("Previous"));
    d->toolPrevious->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    d->toolPrevious->setIcon(QIcon(QString::fromUtf8(":/qt/scripttools/debugging/images/%1/previous.png").arg(system)));
    hboxLayout->addWidget(d->toolPrevious);

    d->toolNext = new QToolButton(this);
    d->toolNext->setAutoRaise(true);
    d->toolNext->setText(tr("Next"));
    d->toolNext->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    d->toolNext->setIcon(QIcon(QString::fromUtf8(":/qt/scripttools/debugging/images/%1/next.png").arg(system)));
    hboxLayout->addWidget(d->toolNext);

    d->checkCase = new QCheckBox(tr("Case Sensitive"), this);
    hboxLayout->addWidget(d->checkCase);

    d->checkWholeWords = new QCheckBox(tr("Whole words"), this);
    hboxLayout->addWidget(d->checkWholeWords);

    d->labelWrapped = new QLabel(this);
    d->labelWrapped->setMinimumSize(QSize(0, 20));
    d->labelWrapped->setMaximumSize(QSize(115, 20));
    d->labelWrapped->setTextFormat(Qt::RichText);
    d->labelWrapped->setScaledContents(true);
    d->labelWrapped->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
    d->labelWrapped->setText(tr("<img src=\":/qt/scripttools/debugging/images/wrap.png\">&nbsp;Search wrapped"));
    hboxLayout->addWidget(d->labelWrapped);

    QSpacerItem *spacerItem = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hboxLayout->addItem(spacerItem);
    setMinimumWidth(minimumSizeHint().width());
    d->labelWrapped->hide();

    d->_q_updateButtons();

    setFocusProxy(d->editFind);
    QObject::connect(d->toolClose, SIGNAL(clicked()), this, SLOT(hide()));
    QObject::connect(d->editFind, SIGNAL(textChanged(QString)),
                     this, SLOT(_q_onTextChanged(QString)));
    QObject::connect(d->toolNext, SIGNAL(clicked()), this, SLOT(_q_next()));
    QObject::connect(d->toolPrevious, SIGNAL(clicked()), this, SLOT(_q_previous()));
}

QScriptDebuggerCodeFinderWidget::~QScriptDebuggerCodeFinderWidget()
{
}

int QScriptDebuggerCodeFinderWidget::findOptions() const
{
    Q_D(const QScriptDebuggerCodeFinderWidget);
    return d->findOptions();
}

QString QScriptDebuggerCodeFinderWidget::text() const
{
    Q_D(const QScriptDebuggerCodeFinderWidget);
    return d->editFind->text();
}

void QScriptDebuggerCodeFinderWidget::setText(const QString &text)
{
    Q_D(const QScriptDebuggerCodeFinderWidget);
    d->editFind->setText(text);
}

void QScriptDebuggerCodeFinderWidget::setOK(bool ok)
{
    Q_D(QScriptDebuggerCodeFinderWidget);
    QPalette p = d->editFind->palette();
    QColor c;
    if (ok)
        c = Qt::white;
    else
        c = QColor(255, 102, 102);
    p.setColor(QPalette::Active, QPalette::Base, c);
    d->editFind->setPalette(p);
    if (!ok)
        d->labelWrapped->hide();
}

void QScriptDebuggerCodeFinderWidget::setWrapped(bool wrapped)
{
    Q_D(QScriptDebuggerCodeFinderWidget);
    d->labelWrapped->setVisible(wrapped);
}

void QScriptDebuggerCodeFinderWidget::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        hide();
    else
        QScriptDebuggerCodeFinderWidgetInterface::keyPressEvent(e);
}

QT_END_NAMESPACE

#include "moc_qscriptdebuggercodefinderwidget_p.cpp"
