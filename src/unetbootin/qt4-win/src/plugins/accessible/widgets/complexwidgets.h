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

#ifndef COMPLEXWIDGETS_H
#define COMPLEXWIDGETS_H

#include <QtCore/qpointer.h>
#include <QtGui/qaccessiblewidget.h>
#include <QtGui/qabstractitemview.h>
#include <QtGui/qaccessible2.h>

QT_BEGIN_NAMESPACE

#ifndef QT_NO_ACCESSIBILITY

class QAbstractButton;
class QHeaderView;
class QTabBar;
class QComboBox;
class QTitleBar;
class QAbstractScrollArea;
class QScrollArea;

#ifndef QT_NO_SCROLLAREA
class QAccessibleAbstractScrollArea : public QAccessibleWidgetEx
{
public:
    explicit QAccessibleAbstractScrollArea(QWidget *widget);

    enum AbstractScrollAreaElement {
        Self = 0,
        Viewport,
        HorizontalContainer,
        VerticalContainer,
        CornerWidget,
        Undefined
    };

    QString text(Text textType, int child) const;
    void setText(Text textType, int child, const QString &text);
    State state(int child) const;
    QVariant invokeMethodEx(QAccessible::Method method, int child, const QVariantList &params);
    int childCount() const;
    int indexOfChild(const QAccessibleInterface *child) const;
    bool isValid() const;
    int navigate(RelationFlag relation, int entry, QAccessibleInterface **target) const;
    QRect rect(int child) const;
    int childAt(int x, int y) const;

//protected:
    QAbstractScrollArea *abstractScrollArea() const;

private:
    QWidgetList accessibleChildren() const;
    AbstractScrollAreaElement elementType(QWidget *widget) const;
    bool isLeftToRight() const;
};

class QAccessibleScrollArea : public QAccessibleAbstractScrollArea
{
public:
    explicit QAccessibleScrollArea(QWidget *widget);
};

#endif // QT_NO_SCROLLAREA

#ifndef QT_NO_ITEMVIEWS
class QAccessibleHeader : public QAccessibleWidgetEx
{
    Q_ACCESSIBLE_OBJECT
public:
    explicit QAccessibleHeader(QWidget *w);

    int childCount() const;

    QRect rect(int child) const;
    QString text(Text t, int child) const;
    Role role(int child) const;
    State state(int child) const;

protected:
    QHeaderView *header() const;
};

class QAccessibleItemRow: public QAccessibleInterface
{
    friend class QAccessibleItemView;
public:
    QAccessibleItemRow(QAbstractItemView *view, const QModelIndex &index = QModelIndex(), bool isHeader = false);
    QRect rect(int child) const;
    QString text(Text t, int child) const;
    void setText(Text t, int child, const QString &text);
    bool isValid() const;
    QObject *object() const;
    Role role(int child) const;
    State state(int child) const;

    int childCount() const;
    int indexOfChild(const QAccessibleInterface *) const;
    QList<QModelIndex> children() const;

    Relation relationTo(int child, const QAccessibleInterface *other, int otherChild) const;
    int childAt(int x, int y) const;
    int navigate(RelationFlag relation, int index, QAccessibleInterface **iface) const;

    int userActionCount(int child) const;
    QString actionText(int action, Text t, int child) const;
    bool doAction(int action, int child, const QVariantList &params = QVariantList());

    QModelIndex childIndex(int child) const;

    QHeaderView *horizontalHeader() const;  //used by QAccessibleItemView
private:
    static QAbstractItemView::CursorAction toCursorAction(Relation rel);
    int logicalFromChild(QHeaderView *header, int child) const;
    int treeLevel() const;
    QHeaderView *verticalHeader() const;
    QString text_helper(int child) const;

    QPersistentModelIndex row;
    QPointer<QAbstractItemView> view;
    bool m_header;
};

class QAccessibleItemView: public QAccessibleAbstractScrollArea, public QAccessibleTableInterface
{
    Q_ACCESSIBLE_OBJECT
public:
    explicit QAccessibleItemView(QWidget *w);

    QObject *object() const;
    Role role(int child) const;
    State state(int child) const;
    QRect rect(int child) const;
    int childAt(int x, int y) const;
    int childCount() const;
    QString text(Text t, int child) const;
    void setText(Text t, int child, const QString &text);
    int indexOfChild(const QAccessibleInterface *iface) const;

    QModelIndex childIndex(int child) const;
    int entryFromIndex(const QModelIndex &index) const;
    bool isValid() const;
    int navigate(RelationFlag relation, int index, QAccessibleInterface **iface) const;

    QAccessibleInterface *accessibleAt(int row, int column);
    QAccessibleInterface *caption();
    int childIndex(int rowIndex, int columnIndex);
    QString columnDescription(int column);
    int columnSpan(int row, int column);
    QAccessibleInterface *columnHeader();
    int columnIndex(int childIndex);
    int columnCount();
    int rowCount();
    int selectedColumnCount();
    int selectedRowCount();
    QString rowDescription(int row);
    int rowSpan(int row, int column);
    QAccessibleInterface *rowHeader();
    int rowIndex(int childIndex);
    int selectedRows(int maxRows, QList<int> *rows);
    int selectedColumns(int maxColumns, QList<int> *columns);
    QAccessibleInterface *summary();
    bool isColumnSelected(int column);
    bool isRowSelected(int row);
    bool isSelected(int row, int column);
    void selectRow(int row);
    void selectColumn(int column);
    void unselectRow(int row);
    void unselectColumn(int column);
    void cellAtIndex(int index, int *row, int *column, int *rowSpan,
                     int *columnSpan, bool *isSelected);

    QHeaderView *horizontalHeader() const;
    QHeaderView *verticalHeader() const;
    bool isValidChildRole(QAccessible::Role role) const;

protected:
    QAbstractItemView *itemView() const;
    QModelIndex index(int row, int column) const;

private:
    inline bool atViewport() const {
        return atVP;
    };
    QAccessible::Role expectedRoleOfChildren() const;

    bool atVP;
};

#endif

#ifndef QT_NO_TABBAR
class QAccessibleTabBar : public QAccessibleWidgetEx
{
    Q_ACCESSIBLE_OBJECT
public:
    explicit QAccessibleTabBar(QWidget *w);

    int childCount() const;

    QRect rect(int child) const;
    QString text(Text t, int child) const;
    Role role(int child) const;
    State state(int child) const;

    bool doAction(int action, int child, const QVariantList &params);
    bool setSelected(int child, bool on, bool extend);
    QVector<int> selection() const;

protected:
    QTabBar *tabBar() const;

private:
    QAbstractButton *button(int child) const;
};
#endif // QT_NO_TABBAR

#ifndef QT_NO_COMBOBOX
class QAccessibleComboBox : public QAccessibleWidgetEx
{
    Q_ACCESSIBLE_OBJECT
public:
    explicit QAccessibleComboBox(QWidget *w);

    enum ComboBoxElements {
        ComboBoxSelf        = 0,
        CurrentText,
        OpenList,
        PopupList
    };

    int childCount() const;
    int childAt(int x, int y) const;
    int indexOfChild(const QAccessibleInterface *child) const;
    int navigate(RelationFlag rel, int entry, QAccessibleInterface **target) const;

    QString text(Text t, int child) const;
    QRect rect(int child) const;
    Role role(int child) const;
    State state(int child) const;

    bool doAction(int action, int child, const QVariantList &params);
    QString actionText(int action, Text t, int child) const;

protected:
    QComboBox *comboBox() const;
};
#endif // QT_NO_COMBOBOX

#endif // QT_NO_ACCESSIBILITY

QT_END_NAMESPACE

#endif // COMPLEXWIDGETS_H
