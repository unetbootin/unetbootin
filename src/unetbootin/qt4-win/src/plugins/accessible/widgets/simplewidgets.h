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

#ifndef SIMPLEWIDGETS_H
#define SIMPLEWIDGETS_H

#include <QtGui/qaccessible2.h>
#include <QtGui/qaccessiblewidget.h>

QT_BEGIN_NAMESPACE

#ifndef QT_NO_ACCESSIBILITY

class QAbstractButton;
class QLineEdit;
class QToolButton;

class QAccessibleButton : public QAccessibleWidgetEx
{
public:
    QAccessibleButton(QWidget *w, Role r);

    QString text(Text t, int child) const;
    State state(int child) const;

    QString actionText(int action, Text text, int child) const;
    bool doAction(int action, int child, const QVariantList &params);

protected:
    QAbstractButton *button() const;
};

#ifndef QT_NO_TOOLBUTTON
class QAccessibleToolButton : public QAccessibleButton
{
public:
    QAccessibleToolButton(QWidget *w, Role role);

    enum ToolButtonElements {
        ToolButtonSelf        = 0,
        ButtonExecute,
        ButtonDropMenu
    };

    Role role(int child) const;
    State state(int child) const;

    int childCount() const;
    QRect rect(int child) const;

    QString text(Text t, int child) const;

    int actionCount(int child) const;
    QString actionText(int action, Text text, int child) const;
    bool doAction(int action, int child, const QVariantList &params);

protected:
    QToolButton *toolButton() const;

    bool isSplitButton() const;
};
#endif // QT_NO_TOOLBUTTON

class QAccessibleDisplay : public QAccessibleWidgetEx
{
public:
    explicit QAccessibleDisplay(QWidget *w, Role role = StaticText);

    QString text(Text t, int child) const;
    Role role(int child) const;

    Relation relationTo(int child, const QAccessibleInterface *other, int otherChild) const;
    int navigate(RelationFlag, int entry, QAccessibleInterface **target) const;
};

#ifndef QT_NO_LINEEDIT
class QAccessibleLineEdit : public QAccessibleWidgetEx, public QAccessibleTextInterface,
                            public QAccessibleSimpleEditableTextInterface
{
public:
    explicit QAccessibleLineEdit(QWidget *o, const QString &name = QString());

    QString text(Text t, int child) const;
    void setText(Text t, int control, const QString &text);
    State state(int child) const;
    QVariant invokeMethodEx(QAccessible::Method method, int child, const QVariantList &params);

    // QAccessibleTextInterface
    void addSelection(int startOffset, int endOffset);
    QString attributes(int offset, int *startOffset, int *endOffset);
    int cursorPosition();
    QRect characterRect(int offset, QAccessible2::CoordinateType coordType);
    int selectionCount();
    int offsetAtPoint(const QPoint &point, QAccessible2::CoordinateType coordType);
    void selection(int selectionIndex, int *startOffset, int *endOffset);
    QString text(int startOffset, int endOffset);
    QString textBeforeOffset (int offset, QAccessible2::BoundaryType boundaryType,
            int *startOffset, int *endOffset);
    QString textAfterOffset(int offset, QAccessible2::BoundaryType boundaryType,
            int *startOffset, int *endOffset);
    QString textAtOffset(int offset, QAccessible2::BoundaryType boundaryType,
            int *startOffset, int *endOffset);
    void removeSelection(int selectionIndex);
    void setCursorPosition(int position);
    void setSelection(int selectionIndex, int startOffset, int endOffset);
    int characterCount();
    void scrollToSubstring(int startIndex, int endIndex);

protected:
    QLineEdit *lineEdit() const;
};
#endif // QT_NO_LINEEDIT

#endif // QT_NO_ACCESSIBILITY

QT_END_NAMESPACE

#endif // SIMPLEWIDGETS_H
