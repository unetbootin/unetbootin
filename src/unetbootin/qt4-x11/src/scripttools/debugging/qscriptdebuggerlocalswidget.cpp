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

#include "qscriptdebuggerlocalswidget_p.h"
#include "qscriptdebuggerlocalswidgetinterface_p_p.h"
#include "qscriptdebuggerlocalsmodel_p.h"
#include "qscriptcompletionproviderinterface_p.h"
#include "qscriptcompletiontaskinterface_p.h"

#include <QtCore/qdebug.h>
#include <QtGui/qheaderview.h>
#include <QtGui/qcompleter.h>
#include <QtGui/qstringlistmodel.h>
#include <QtGui/qtreeview.h>
#include <QtGui/qboxlayout.h>
#include <QtGui/qsortfilterproxymodel.h>
#include <QtGui/qlineedit.h>
#include <QtGui/qstyleditemdelegate.h>
#include <QtGui/qevent.h>
#include <QtGui/qmessagebox.h>
#include <QtScript/qscriptengine.h>

QT_BEGIN_NAMESPACE

namespace {

class CustomProxyModel : public QSortFilterProxyModel
{
public:
    CustomProxyModel(QObject *parent = 0)
        : QSortFilterProxyModel(parent) {}

    bool hasChildren(const QModelIndex &parent) const
    {
        QModelIndex sourceParent = mapToSource(parent);
        if (parent.isValid() && !sourceParent.isValid())
            return false;
        return sourceModel()->hasChildren(sourceParent);
    }
};

} // namespace

class QScriptDebuggerLocalsWidgetPrivate
    : public QScriptDebuggerLocalsWidgetInterfacePrivate
{
    Q_DECLARE_PUBLIC(QScriptDebuggerLocalsWidget)
public:
    QScriptDebuggerLocalsWidgetPrivate();
    ~QScriptDebuggerLocalsWidgetPrivate();

    void complete(QLineEdit *le);

    // private slots
    void _q_onCompletionTaskFinished();
    void _q_insertCompletion(const QString &text);
    void _q_expandIndex(const QModelIndex &index);

    QTreeView *view;
    QPointer<QLineEdit> completingEditor;
    QCompleter *completer;
    CustomProxyModel *proxy;
};

QScriptDebuggerLocalsWidgetPrivate::QScriptDebuggerLocalsWidgetPrivate()
{
    completingEditor = 0;
    completer = 0;
    proxy = 0;
}

QScriptDebuggerLocalsWidgetPrivate::~QScriptDebuggerLocalsWidgetPrivate()
{
}

void QScriptDebuggerLocalsWidgetPrivate::complete(QLineEdit *le)
{
    Q_Q(QScriptDebuggerLocalsWidget);
    QScriptCompletionTaskInterface *task = 0;
    // ### need to pass the current frame #
    task = completionProvider->createCompletionTask(
        le->text(), le->cursorPosition(),
        q->localsModel()->frameIndex(), /*options=*/0);
    QObject::connect(task, SIGNAL(finished()),
                     q, SLOT(_q_onCompletionTaskFinished()));
    completingEditor = le;
    task->start();
}

void QScriptDebuggerLocalsWidgetPrivate::_q_onCompletionTaskFinished()
{
    Q_Q(QScriptDebuggerLocalsWidget);
    QScriptCompletionTaskInterface *task = 0;
    task = qobject_cast<QScriptCompletionTaskInterface*>(q_func()->sender());
    if (!completingEditor) {
        task->deleteLater();
        return;
    }

    if (task->resultCount() == 1) {
        // do the completion right away
        QString completion = task->resultAt(0);
        completion.append(task->appendix());
        QString tmp = completingEditor->text();
        tmp.remove(task->position(), task->length());
        tmp.insert(task->position(), completion);
        completingEditor->setText(tmp);
        completingEditor = 0;
    } else if (task->resultCount() > 1) {
        // popup completion
        if (!completer) {
            completer = new QCompleter(q);
            completer->setCompletionMode(QCompleter::PopupCompletion);
            completer->setCaseSensitivity(Qt::CaseSensitive);
            completer->setWrapAround(false);
            QObject::connect(completer, SIGNAL(activated(QString)),
                             q, SLOT(_q_insertCompletion(QString)));
        }
        QStringListModel *model = qobject_cast<QStringListModel*>(completer->model());
        if (!model) {
            model = new QStringListModel(q);
            completer->setModel(model);
        }
        QStringList strings;
        for (int i = 0; i < task->resultCount(); ++i)
            strings.append(task->resultAt(i));
        model->setStringList(strings);
        QString prefix = completingEditor->text().mid(task->position(), task->length());
        completer->setCompletionPrefix(prefix);
        completingEditor->setCompleter(completer);
        // we want to handle the insertion ourselves
        QObject::disconnect(completer, 0, completingEditor, 0);
        completer->complete();
    }
    task->deleteLater();
}

void QScriptDebuggerLocalsWidgetPrivate::_q_insertCompletion(const QString &text)
{
    Q_ASSERT(completingEditor != 0);
    QString tmp = completingEditor->text();
    tmp.insert(completingEditor->cursorPosition(), text.mid(completer->completionPrefix().length()));
    completingEditor->setText(tmp);
    completingEditor = 0;
}

void QScriptDebuggerLocalsWidgetPrivate::_q_expandIndex(const QModelIndex &index)
{
    view->expand(proxy->mapFromSource(index));
}

class QScriptDebuggerLocalsItemDelegate
    : public QStyledItemDelegate
{
    Q_OBJECT
public:
    QScriptDebuggerLocalsItemDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    bool eventFilter(QObject *watched, QEvent *event);

private Q_SLOTS:
    void validateInput(const QString &text)
    {
        QWidget *editor = qobject_cast<QWidget*>(sender());
        QPalette pal = editor->palette();
        QColor col;
#if QT_VERSION >= 0x040500
        bool ok = (QScriptEngine::checkSyntax(text).state() == QScriptSyntaxCheckResult::Valid);
#else
        bool ok = true;
#endif
        if (ok) {
            col = Qt::white;
        } else {
            QScriptSyntaxCheckResult result = QScriptEngine::checkSyntax(
                text + QLatin1Char('\n'));
            if (result.state() == QScriptSyntaxCheckResult::Intermediate)
                col = QColor(255, 240, 192);
            else
                col = QColor(255, 102, 102);
        }
        pal.setColor(QPalette::Active, QPalette::Base, col);
        editor->setPalette(pal);
    }

private:
    static const QWidget *widget(const QStyleOptionViewItem &option)
    {
        if (const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option))
            return v3->widget;
        return 0;
    }
};

QScriptDebuggerLocalsItemDelegate::QScriptDebuggerLocalsItemDelegate(
    QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QWidget *QScriptDebuggerLocalsItemDelegate::createEditor(
    QWidget *parent, const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    QWidget *editor = QStyledItemDelegate::createEditor(parent, option, index);
    if (index.column() == 1) {
        // value
        QLineEdit *le = qobject_cast<QLineEdit*>(editor);
        if (le) {
            QObject::connect(le, SIGNAL(textEdited(QString)),
                             this, SLOT(validateInput(QString)));
        }
    }
    return editor;
}

bool QScriptDebuggerLocalsItemDelegate::eventFilter(QObject *watched, QEvent *event)
{
    QLineEdit *le = qobject_cast<QLineEdit*>(watched);
    if (!le)
        return QStyledItemDelegate::eventFilter(watched, event);

    QScriptDebuggerLocalsWidget *localsWidget = qobject_cast<QScriptDebuggerLocalsWidget*>(parent());
    Q_ASSERT(localsWidget != 0);
    QScriptDebuggerLocalsWidgetPrivate *lvp =
        reinterpret_cast<QScriptDebuggerLocalsWidgetPrivate*>(
            QScriptDebuggerLocalsWidgetPrivate::get(localsWidget));

    if ((event->type() == QEvent::FocusIn) && lvp->completingEditor) {
        // because QLineEdit insists on being difficult...
        return true;
    }

    if (event->type() != QEvent::KeyPress)
        return QStyledItemDelegate::eventFilter(watched, event);
    QKeyEvent *ke = static_cast<QKeyEvent*>(event);
    if ((ke->key() == Qt::Key_Enter) || (ke->key() == Qt::Key_Return)) {
#if QT_VERSION >= 0x040500
        if (QScriptEngine::checkSyntax(le->text()).state() != QScriptSyntaxCheckResult::Valid) {
            // ignore when script contains syntax error
            return true;
        }
#endif
    }
    if (ke->key() != Qt::Key_Tab)
        return QStyledItemDelegate::eventFilter(watched, event);

    // trigger completion
    lvp->complete(le);
    return true;
}

void QScriptDebuggerLocalsItemDelegate::setModelData(
    QWidget *editor, QAbstractItemModel *model,
    const QModelIndex &index) const
{
#if QT_VERSION >= 0x040500
    if (index.column() == 1) {
        // check that the syntax is OK
        QString expression = qobject_cast<QLineEdit*>(editor)->text();
        if (QScriptEngine::checkSyntax(expression).state() != QScriptSyntaxCheckResult::Valid)
            return;
    }
#endif
    QStyledItemDelegate::setModelData(editor, model, index);
}

void QScriptDebuggerLocalsItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                              const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);
#if 0
    QModelIndex parent = index.parent();
    if (parent.isValid()) {
        QStyledItemDelegate::paint(painter, option, index);
    } else {
        // this is a top-level item.
        const QTreeView *view = qobject_cast<const QTreeView*>(widget(option));
        Q_ASSERT(view != 0);

        QStyleOptionButton buttonOption;

        buttonOption.state = option.state;
#ifdef Q_WS_MAC
        buttonOption.state |= QStyle::State_Raised;
#endif
        buttonOption.state &= ~QStyle::State_HasFocus;

        buttonOption.rect = option.rect;
        buttonOption.palette = option.palette;
        buttonOption.features = QStyleOptionButton::None;
        view->style()->drawControl(QStyle::CE_PushButton, &buttonOption, painter, view);

        QStyleOption branchOption;
        static const int i = 9; // ### hardcoded in qcommonstyle.cpp
        QRect r = option.rect;
        branchOption.rect = QRect(r.left() + i/2, r.top() + (r.height() - i)/2, i, i);
        branchOption.palette = option.palette;
        branchOption.state = QStyle::State_Children;

        if (view->isExpanded(index))
            branchOption.state |= QStyle::State_Open;

        view->style()->drawPrimitive(QStyle::PE_IndicatorBranch, &branchOption, painter, view);

        // draw text
        QRect textrect = QRect(r.left() + i*2, r.top(), r.width() - ((5*i)/2), r.height());
        QString text = elidedText(option.fontMetrics, textrect.width(), Qt::ElideMiddle, 
            index.data(Qt::DisplayRole).toString());
        view->style()->drawItemText(painter, textrect, Qt::AlignCenter,
                                    option.palette, view->isEnabled(), text);
    }
#endif
}

QScriptDebuggerLocalsWidget::QScriptDebuggerLocalsWidget(QWidget *parent)
    : QScriptDebuggerLocalsWidgetInterface(*new QScriptDebuggerLocalsWidgetPrivate, parent, 0)
{
    Q_D(QScriptDebuggerLocalsWidget);
    d->view = new QTreeView();
    d->view->setItemDelegate(new QScriptDebuggerLocalsItemDelegate(this));
    d->view->setEditTriggers(QAbstractItemView::DoubleClicked);
//    d->view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    d->view->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
    d->view->setAlternatingRowColors(true);
    d->view->setSelectionBehavior(QAbstractItemView::SelectRows);
    d->view->setSortingEnabled(true);
    d->view->header()->setDefaultAlignment(Qt::AlignLeft);
//    d->view->header()->setSortIndicatorShown(true);
//    d->view->header()->setResizeMode(QHeaderView::ResizeToContents);

    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->setMargin(0);
    vbox->addWidget(d->view);
}

QScriptDebuggerLocalsWidget::~QScriptDebuggerLocalsWidget()
{
}

/*!
  \reimp
*/
QScriptDebuggerLocalsModel *QScriptDebuggerLocalsWidget::localsModel() const
{
    Q_D(const QScriptDebuggerLocalsWidget);
    if (!d->proxy)
        return 0;
    return qobject_cast<QScriptDebuggerLocalsModel*>(d->proxy->sourceModel());
}

/*!
  \reimp
*/
void QScriptDebuggerLocalsWidget::setLocalsModel(QScriptDebuggerLocalsModel *model)
{
    Q_D(QScriptDebuggerLocalsWidget);
    if (localsModel()) {
        QObject::disconnect(localsModel(), 0, d->view, 0);
    }
    if (model) {
        QObject::connect(model, SIGNAL(scopeObjectAvailable(QModelIndex)),
                         this, SLOT(_q_expandIndex(QModelIndex)));
    }
    if (!d->proxy) {
        d->proxy = new CustomProxyModel(this);
        d->view->sortByColumn(0, Qt::AscendingOrder);
    }
    d->proxy->setSourceModel(model);
    d->view->setModel(d->proxy);
}

/*!
  \reimp
*/
void QScriptDebuggerLocalsWidget::expand(const QModelIndex &index)
{
    Q_D(QScriptDebuggerLocalsWidget);
    d->view->expand(index);
    d->view->setFirstColumnSpanned(index.row(), QModelIndex(), true);
}

QT_END_NAMESPACE

#include "qscriptdebuggerlocalswidget.moc"

#include "moc_qscriptdebuggerlocalswidget_p.cpp"
