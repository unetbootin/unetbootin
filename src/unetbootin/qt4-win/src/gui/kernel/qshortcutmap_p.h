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

#ifndef QSHORTCUTMAP_P_H
#define QSHORTCUTMAP_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "QtGui/qkeysequence.h"
#include "QtCore/qvector.h"

QT_BEGIN_NAMESPACE

#ifndef QT_NO_SHORTCUT

// To enable dump output uncomment below
//#define Dump_QShortcutMap

class QKeyEvent;
struct QShortcutEntry;
class QShortcutMapPrivate;
class QWidget;
class QAction;
class QObject;

class QShortcutMap
{
    Q_DECLARE_PRIVATE(QShortcutMap)
public:
    QShortcutMap();
    ~QShortcutMap();

    int addShortcut(QObject *owner, const QKeySequence &key, Qt::ShortcutContext context);
    int removeShortcut(int id, QObject *owner, const QKeySequence &key = QKeySequence());
    int setShortcutEnabled(bool enable, int id, QObject *owner, const QKeySequence &key = QKeySequence());
    int setShortcutAutoRepeat(bool on, int id, QObject *owner, const QKeySequence &key = QKeySequence());

    void resetState();
    QKeySequence::SequenceMatch nextState(QKeyEvent *e);
    QKeySequence::SequenceMatch state();
    void dispatchEvent(QKeyEvent *e);
    bool tryShortcutEvent(QWidget *w, QKeyEvent *e);

#ifdef Dump_QShortcutMap
    void dumpMap() const;
#endif

private:
    bool correctContext(Qt::ShortcutContext context, QWidget *w, QWidget *active_window);
#ifndef QT_NO_ACTION
    bool correctContext(Qt::ShortcutContext context,QAction *a, QWidget *active_window);
#endif
    QShortcutMapPrivate *d_ptr;

    QKeySequence::SequenceMatch find(QKeyEvent *e);
    QKeySequence::SequenceMatch matches(const QKeySequence &seq1, const QKeySequence &seq2) const;
    QVector<const QShortcutEntry *> matches() const;
    void createNewSequences(QKeyEvent *e, QVector<QKeySequence> &ksl);
    void clearSequence(QVector<QKeySequence> &ksl);
    bool correctContext(const QShortcutEntry &item);
    int translateModifiers(Qt::KeyboardModifiers modifiers);
};

#endif // QT_NO_SHORTCUT

QT_END_NAMESPACE

#endif // QSHORTCUTMAP_P_H
