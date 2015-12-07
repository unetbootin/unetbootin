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

#ifndef QACTIONGROUP_H
#define QACTIONGROUP_H

#include <QtGui/qaction.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#ifndef QT_NO_ACTION

class QActionGroupPrivate;

class Q_GUI_EXPORT QActionGroup : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QActionGroup)

    Q_PROPERTY(bool exclusive READ isExclusive WRITE setExclusive)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible)

public:
    explicit QActionGroup(QObject* parent);
    ~QActionGroup();

    QAction *addAction(QAction* a);
    QAction *addAction(const QString &text);
    QAction *addAction(const QIcon &icon, const QString &text);
    void removeAction(QAction *a);
    QList<QAction*> actions() const;

    QAction *checkedAction() const;
    bool isExclusive() const;
    bool isEnabled() const;
    bool isVisible() const;

#ifdef QT3_SUPPORT
    inline QT3_SUPPORT void add(QAction* a) { addAction(a); }
    inline QT3_SUPPORT void addSeparator()
    { QAction *act = new QAction(this); act->setSeparator(true); addAction(act); }
    inline QT3_SUPPORT bool addTo(QWidget *w) { w->addActions(actions()); return true; }
#endif

public Q_SLOTS:
    void setEnabled(bool);
    inline void setDisabled(bool b) { setEnabled(!b); }
    void setVisible(bool);
    void setExclusive(bool);

Q_SIGNALS:
    void triggered(QAction *);
    QT_MOC_COMPAT void selected(QAction *);
    void hovered(QAction *);

private:
    Q_DISABLE_COPY(QActionGroup)
    Q_PRIVATE_SLOT(d_func(), void _q_actionTriggered())
    Q_PRIVATE_SLOT(d_func(), void _q_actionChanged())
    Q_PRIVATE_SLOT(d_func(), void _q_actionHovered())
};

#endif // QT_NO_ACTION

QT_END_NAMESPACE

QT_END_HEADER

#endif // QACTIONGROUP_H
