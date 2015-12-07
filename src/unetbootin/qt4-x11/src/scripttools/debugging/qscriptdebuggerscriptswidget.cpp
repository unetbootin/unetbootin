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

#include "qscriptdebuggerscriptswidget_p.h"
#include "qscriptdebuggerscriptswidgetinterface_p_p.h"
#include "qscriptdebuggerscriptsmodel_p.h"

#include <QtCore/qdebug.h>
#include <QtGui/qheaderview.h>
#include <QtGui/qtreeview.h>
#include <QtGui/qboxlayout.h>

QT_BEGIN_NAMESPACE

class QScriptDebuggerScriptsWidgetPrivate
    : public QScriptDebuggerScriptsWidgetInterfacePrivate
{
    Q_DECLARE_PUBLIC(QScriptDebuggerScriptsWidget)
public:
    QScriptDebuggerScriptsWidgetPrivate();
    ~QScriptDebuggerScriptsWidgetPrivate();

    // private slots
    void _q_onCurrentChanged(const QModelIndex &index);

    QTreeView *view;
    qint64 currentScriptId;
};

QScriptDebuggerScriptsWidgetPrivate::QScriptDebuggerScriptsWidgetPrivate()
{
    currentScriptId = -1;
}

QScriptDebuggerScriptsWidgetPrivate::~QScriptDebuggerScriptsWidgetPrivate()
{
}

void QScriptDebuggerScriptsWidgetPrivate::_q_onCurrentChanged(const QModelIndex &index)
{
    Q_Q(QScriptDebuggerScriptsWidget);
    if (!index.isValid())
        return;
    qint64 sid = q->scriptsModel()->scriptIdFromIndex(index);
    if (sid != -1) {
        if (currentScriptId != sid) {
            currentScriptId = sid;
            emit q->currentScriptChanged(sid);
        }
    } else {
        qint64 sid = q->scriptsModel()->scriptIdFromIndex(index.parent());
        Q_ASSERT(sid != -1);
        currentScriptId = sid;
        emit q->currentScriptChanged(sid);
        QPair<QString, int> info = q->scriptsModel()->scriptFunctionInfoFromIndex(index);
        emit q->scriptLocationSelected(info.second);
    }
}

QScriptDebuggerScriptsWidget::QScriptDebuggerScriptsWidget(QWidget *parent)
    : QScriptDebuggerScriptsWidgetInterface(*new QScriptDebuggerScriptsWidgetPrivate, parent, 0)
{
    Q_D(QScriptDebuggerScriptsWidget);
    d->view = new QTreeView();
    d->view->setEditTriggers(QAbstractItemView::NoEditTriggers);
//    d->view->setAlternatingRowColors(true);
//    d->view->setRootIsDecorated(false);
    d->view->setSelectionBehavior(QAbstractItemView::SelectRows);
    d->view->header()->hide();
//    d->view->header()->setDefaultAlignment(Qt::AlignLeft);
//    d->view->header()->setResizeMode(QHeaderView::ResizeToContents);

    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->setMargin(0);
    vbox->addWidget(d->view);
}

QScriptDebuggerScriptsWidget::~QScriptDebuggerScriptsWidget()
{
}

/*!
  \reimp
*/
QScriptDebuggerScriptsModel *QScriptDebuggerScriptsWidget::scriptsModel() const
{
    Q_D(const QScriptDebuggerScriptsWidget);
    return qobject_cast<QScriptDebuggerScriptsModel*>(d->view->model());
}

/*!
  \reimp
*/
void QScriptDebuggerScriptsWidget::setScriptsModel(QScriptDebuggerScriptsModel *model)
{
    Q_D(QScriptDebuggerScriptsWidget);
    d->view->setModel(model);
    QObject::connect(d->view->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                     this, SLOT(_q_onCurrentChanged(QModelIndex)));
}

qint64 QScriptDebuggerScriptsWidget::currentScriptId() const
{
    Q_D(const QScriptDebuggerScriptsWidget);
    return scriptsModel()->scriptIdFromIndex(d->view->currentIndex());
}

void QScriptDebuggerScriptsWidget::setCurrentScript(qint64 id)
{
    Q_D(QScriptDebuggerScriptsWidget);
    d->view->setCurrentIndex(scriptsModel()->indexFromScriptId(id));
}

QT_END_NAMESPACE

#include "moc_qscriptdebuggerscriptswidget_p.cpp"
