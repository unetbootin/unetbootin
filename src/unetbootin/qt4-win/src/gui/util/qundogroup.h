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

#ifndef QUNDOGROUP_H
#define QUNDOGROUP_H

#include <QtCore/qobject.h>
#include <QtCore/qstring.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QUndoGroupPrivate;
class QUndoStack;
class QAction;

QT_MODULE(Gui)

#ifndef QT_NO_UNDOGROUP

class Q_GUI_EXPORT QUndoGroup : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QUndoGroup)

public:
    explicit QUndoGroup(QObject *parent = 0);

    void addStack(QUndoStack *stack);
    void removeStack(QUndoStack *stack);
    QList<QUndoStack*> stacks() const;
    QUndoStack *activeStack() const;

#ifndef QT_NO_ACTION
    QAction *createUndoAction(QObject *parent,
                                const QString &prefix = QString()) const;
    QAction *createRedoAction(QObject *parent,
                                const QString &prefix = QString()) const;
#endif // QT_NO_ACTION
    bool canUndo() const;
    bool canRedo() const;
    QString undoText() const;
    QString redoText() const;
    bool isClean() const;

public Q_SLOTS:
    void undo();
    void redo();
    void setActiveStack(QUndoStack *stack);

Q_SIGNALS:
    void activeStackChanged(QUndoStack *stack);
    void indexChanged(int idx);
    void cleanChanged(bool clean);
    void canUndoChanged(bool canUndo);
    void canRedoChanged(bool canRedo);
    void undoTextChanged(const QString &undoText);
    void redoTextChanged(const QString &redoText);

private:
    Q_DISABLE_COPY(QUndoGroup)
};

#endif // QT_NO_UNDOGROUP

QT_END_NAMESPACE

QT_END_HEADER

#endif // QUNDOGROUP_H
