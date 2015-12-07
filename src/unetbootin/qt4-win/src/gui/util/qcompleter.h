/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QCOMPLETER_H
#define QCOMPLETER_H

#include <QtCore/qobject.h>
#include <QtCore/qpoint.h>
#include <QtCore/qstring.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qrect.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#ifndef QT_NO_COMPLETER

class QCompleterPrivate;
class QAbstractItemView;
class QAbstractProxyModel;
class QWidget;

class Q_GUI_EXPORT QCompleter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString completionPrefix READ completionPrefix WRITE setCompletionPrefix)
    Q_PROPERTY(ModelSorting modelSorting READ modelSorting WRITE setModelSorting)
    Q_PROPERTY(CompletionMode completionMode READ completionMode WRITE setCompletionMode)
    Q_PROPERTY(int completionColumn READ completionColumn WRITE setCompletionColumn)
    Q_PROPERTY(int completionRole READ completionRole WRITE setCompletionRole)
    Q_PROPERTY(Qt::CaseSensitivity caseSensitivity READ caseSensitivity WRITE setCaseSensitivity)
    Q_PROPERTY(bool wrapAround READ wrapAround WRITE setWrapAround)

public:
    enum CompletionMode {
        PopupCompletion,
        UnfilteredPopupCompletion,
        InlineCompletion
    };

    enum ModelSorting {
        UnsortedModel = 0,
        CaseSensitivelySortedModel,
        CaseInsensitivelySortedModel
    };

    QCompleter(QObject *parent = 0);
    QCompleter(QAbstractItemModel *model, QObject *parent = 0);
#ifndef QT_NO_STRINGLISTMODEL
    QCompleter(const QStringList& completions, QObject *parent = 0);
#endif
    ~QCompleter();

    void setWidget(QWidget *widget);
    QWidget *widget() const;

    void setModel(QAbstractItemModel *c);
    QAbstractItemModel *model() const;

    void setCompletionMode(CompletionMode mode);
    CompletionMode completionMode() const;

    QAbstractItemView *popup() const;
    void setPopup(QAbstractItemView *popup);

    void setCaseSensitivity(Qt::CaseSensitivity caseSensitivity);
    Qt::CaseSensitivity caseSensitivity() const;

    void setModelSorting(ModelSorting sorting);
    ModelSorting modelSorting() const;

    void setCompletionColumn(int column);
    int  completionColumn() const;

    void setCompletionRole(int role);
    int  completionRole() const;

    bool wrapAround() const;

    int completionCount() const;
    bool setCurrentRow(int row);
    int currentRow() const;

    QModelIndex currentIndex() const;
    QString currentCompletion() const;

    QAbstractItemModel *completionModel() const;

    QString completionPrefix() const;

public Q_SLOTS:
    void setCompletionPrefix(const QString &prefix);
    void complete(const QRect& rect = QRect());
    void setWrapAround(bool wrap);

public:
    virtual QString pathFromIndex(const QModelIndex &index) const;
    virtual QStringList splitPath(const QString &path) const;

protected:
    bool eventFilter(QObject *o, QEvent *e);
    bool event(QEvent *);

Q_SIGNALS:
    void activated(const QString &text);
    void activated(const QModelIndex &index);
    void highlighted(const QString &text);
    void highlighted(const QModelIndex &index);

private:
    Q_DISABLE_COPY(QCompleter)
    Q_DECLARE_PRIVATE(QCompleter)

    Q_PRIVATE_SLOT(d_func(), void _q_complete(QModelIndex))
    Q_PRIVATE_SLOT(d_func(), void _q_completionSelected(const QItemSelection&))
    Q_PRIVATE_SLOT(d_func(), void _q_autoResizePopup())
};

#endif // QT_NO_COMPLETER

QT_END_NAMESPACE

QT_END_HEADER

#endif // QCOMPLETER_H
