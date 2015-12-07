/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the plugins of the Qt Toolkit.
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

#ifndef QACCESSIBLEMENU_H
#define QACCESSIBLEMENU_H

#include <QtGui/qaccessiblewidget.h>

QT_BEGIN_NAMESPACE

#ifndef QT_NO_ACCESSIBILITY

#ifndef QT_NO_MENU
class QMenu;
class QMenuBar;
class QAction;

class QAccessibleMenu : public QAccessibleWidgetEx
{
public:
    explicit QAccessibleMenu(QWidget *w);

    int childCount() const;
    int childAt(int x, int y) const;

    QRect rect(int child) const;
    QString text(Text t, int child) const;
    Role role(int child) const;
    State state(int child) const;
    int navigate(RelationFlag relation, int entry, QAccessibleInterface **target) const;
    int indexOfChild( const QAccessibleInterface *child ) const;

    QString actionText(int action, QAccessible::Text text, int child) const;
    bool doAction(int action, int child, const QVariantList &params);

protected:
    QMenu *menu() const;
};

#ifndef QT_NO_MENUBAR
class QAccessibleMenuBar : public QAccessibleWidgetEx
{
public:
    explicit QAccessibleMenuBar(QWidget *w);

    int childCount() const;
    int childAt(int x, int y) const;

    QRect rect(int child) const;
    QString text(Text t, int child) const;
    Role role(int child) const;
    State state(int child) const;
    int navigate(RelationFlag relation, int entry, QAccessibleInterface **target) const;
    int indexOfChild( const QAccessibleInterface *child ) const;

    QString actionText(int action, QAccessible::Text text, int child) const;
    bool doAction(int action, int child, const QVariantList &params);

protected:
    QMenuBar *menuBar() const;
};
#endif // QT_NO_MENUBAR



class QAccessibleMenuItem : public QAccessibleInterface
{
public:
    explicit QAccessibleMenuItem(QWidget *owner, QAction *w);

    virtual ~QAccessibleMenuItem();
    virtual QString actionText ( int action, Text t, int child ) const;
    virtual int childAt ( int x, int y ) const;
    virtual int childCount () const;
    virtual bool doAction ( int action, int child, const QVariantList & params = QVariantList() );
    virtual int indexOfChild ( const QAccessibleInterface * child ) const;
    virtual bool isValid () const;
    virtual int navigate ( RelationFlag relation, int entry, QAccessibleInterface ** target ) const;
    virtual QObject * object () const;
    virtual QRect rect ( int child ) const;
    virtual Relation relationTo ( int child, const QAccessibleInterface * other, int otherChild ) const;
    virtual Role role ( int child ) const;
    virtual void setText ( Text t, int child, const QString & text );
    virtual State state ( int child ) const;
    virtual QString text ( Text t, int child ) const;
    virtual int userActionCount ( int child ) const;

    QWidget *owner() const;


protected:
    QAction *action() const;
private:
    QAction *m_action;
    QWidget *m_owner; // can hold either QMenu or the QMenuBar that contains the action
};

#endif // QT_NO_MENU

QT_END_NAMESPACE
#endif // QT_NO_ACCESSIBILITY
#endif // QACCESSIBLEMENU_H
