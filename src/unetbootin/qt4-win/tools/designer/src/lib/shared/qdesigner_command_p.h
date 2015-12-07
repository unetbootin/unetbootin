/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of Qt Designer.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#ifndef QDESIGNER_COMMAND_H
#define QDESIGNER_COMMAND_H

#include "shared_global_p.h"
#include "shared_enums_p.h"
#include "layoutinfo_p.h"
#include "qdesigner_utils_p.h"
#include "qdesigner_formwindowcommand_p.h"
#include "qdesigner_formeditorcommand_p.h"

#include <QtDesigner/layoutdecoration.h>

#include <QtGui/QIcon>
#include <QtCore/QObject>
#include <QtCore/QPair>
#include <QtCore/QMap>
#include <QtCore/QPoint>

QT_BEGIN_NAMESPACE

class QDesignerContainerExtension;
class QDesignerMetaDataBaseItemInterface;
class QDesignerMenu;

class QMenuBar;
class QStatusBar;
class QToolBar;
class QToolBox;
class QTabWidget;
class QTableWidget;
class QTableWidgetItem;
class QTreeWidget;
class QTreeWidgetItem;
class QListWidget;
class QComboBox;
class QStackedWidget;
class QDockWidget;
class QMainWindow;

namespace qdesigner_internal {

class Layout;
class LayoutHelper;
class PropertySheetIconValue;
class DesignerIconCache;
struct LayoutProperties;

class QDESIGNER_SHARED_EXPORT InsertWidgetCommand: public QDesignerFormWindowCommand
{

public:
    explicit InsertWidgetCommand(QDesignerFormWindowInterface *formWindow);
    ~InsertWidgetCommand();

    void init(QWidget *widget, bool already_in_form = false);

    virtual void redo();
    virtual void undo();

private:
    void refreshBuddyLabels();

    QPointer<QWidget> m_widget;
    QDesignerLayoutDecorationExtension::InsertMode m_insertMode;
    QPair<int, int> m_cell;
    LayoutHelper* m_layoutHelper;
    bool m_widgetWasManaged;
};

class QDESIGNER_SHARED_EXPORT ChangeZOrderCommand: public QDesignerFormWindowCommand
{

public:
    explicit ChangeZOrderCommand(QDesignerFormWindowInterface *formWindow);

    void init(QWidget *widget);

    virtual void redo();
    virtual void undo();
protected:
    virtual QWidgetList reorderWidget(const QWidgetList &list, QWidget *widget) const = 0;
    virtual void reorder(QWidget *widget) const = 0;

private:
    QPointer<QWidget> m_widget;
    QPointer<QWidget> m_oldPreceding;
    QList<QWidget *> m_oldParentZOrder;
};

class QDESIGNER_SHARED_EXPORT RaiseWidgetCommand: public ChangeZOrderCommand
{

public:
    explicit RaiseWidgetCommand(QDesignerFormWindowInterface *formWindow);

    void init(QWidget *widget);

protected:
    virtual QWidgetList reorderWidget(const QWidgetList &list, QWidget *widget) const;
    virtual void reorder(QWidget *widget) const;
};

class QDESIGNER_SHARED_EXPORT LowerWidgetCommand: public ChangeZOrderCommand
{

public:
    explicit LowerWidgetCommand(QDesignerFormWindowInterface *formWindow);

    void init(QWidget *widget);

protected:
    virtual QWidgetList reorderWidget(const QWidgetList &list, QWidget *widget) const;
    virtual void reorder(QWidget *widget) const;
};

class QDESIGNER_SHARED_EXPORT AdjustWidgetSizeCommand: public QDesignerFormWindowCommand
{

public:
    explicit AdjustWidgetSizeCommand(QDesignerFormWindowInterface *formWindow);

    void init(QWidget *widget);

    virtual void redo();
    virtual void undo();

private:
    QWidget *widgetForAdjust() const;
    void updatePropertyEditor() const;

    QPointer<QWidget> m_widget;
    QSize m_size;
};

// Helper to correctly unmanage a widget and its children for delete operations
class  QDESIGNER_SHARED_EXPORT ManageWidgetCommandHelper {
public:
    typedef QVector<QWidget*> WidgetVector;

    ManageWidgetCommandHelper();
    void init(const QDesignerFormWindowInterface *fw, QWidget *widget);
    void init(QWidget *widget, const WidgetVector &managedChildren);

    void manage(QDesignerFormWindowInterface *fw);
    void unmanage(QDesignerFormWindowInterface *fw);

    const WidgetVector &managedChildren() const { return m_managedChildren; }
private:
    QWidget *m_widget;
    WidgetVector m_managedChildren;
};

class QDESIGNER_SHARED_EXPORT DeleteWidgetCommand: public QDesignerFormWindowCommand
{

public:
    explicit DeleteWidgetCommand(QDesignerFormWindowInterface *formWindow);
    ~DeleteWidgetCommand();

    enum DeleteFlags { DoNotUnmanage = 0x1, DoNotSimplifyLayout = 0x2 };

    void init(QWidget *widget, unsigned flags = 0);

    virtual void redo();
    virtual void undo();

private:
    QPointer<QWidget> m_widget;
    QPointer<QWidget> m_parentWidget;
    QRect m_geometry;
    LayoutInfo::Type m_layoutType;
    LayoutHelper* m_layoutHelper;
    unsigned m_flags;
    QRect m_layoutPosition;
    int m_splitterIndex;
    bool m_layoutSimplified;
    QDesignerMetaDataBaseItemInterface *m_formItem;
    int m_tabOrderIndex;
    ManageWidgetCommandHelper m_manageHelper;
};

class QDESIGNER_SHARED_EXPORT ReparentWidgetCommand: public QDesignerFormWindowCommand
{

public:
    explicit ReparentWidgetCommand(QDesignerFormWindowInterface *formWindow);

    void init(QWidget *widget, QWidget *parentWidget);

    virtual void redo();
    virtual void undo();

private:
    QPointer<QWidget> m_widget;
    QPoint m_oldPos;
    QPoint m_newPos;
    QPointer<QWidget> m_oldParentWidget;
    QPointer<QWidget> m_newParentWidget;
    QList<QWidget *> m_oldParentList;
    QList<QWidget *> m_oldParentZOrder;
};

class QDESIGNER_SHARED_EXPORT ChangeLayoutItemGeometry: public QDesignerFormWindowCommand
{

public:
    explicit ChangeLayoutItemGeometry(QDesignerFormWindowInterface *formWindow);

    void init(QWidget *widget, int row, int column, int rowspan, int colspan);

    virtual void redo();
    virtual void undo();

protected:
    void changeItemPosition(const QRect &g);

private:
    QPointer<QWidget> m_widget;
    QRect m_oldInfo;
    QRect m_newInfo;
};

class QDESIGNER_SHARED_EXPORT TabOrderCommand: public QDesignerFormWindowCommand
{

public:
    explicit TabOrderCommand(QDesignerFormWindowInterface *formWindow);

    void init(const QList<QWidget*> &newTabOrder);

    inline QList<QWidget*> oldTabOrder() const
    { return m_oldTabOrder; }

    inline QList<QWidget*> newTabOrder() const
    { return m_newTabOrder; }

    virtual void redo();
    virtual void undo();

private:
    QDesignerMetaDataBaseItemInterface *m_widgetItem;
    QList<QWidget*> m_oldTabOrder;
    QList<QWidget*> m_newTabOrder;
};

class QDESIGNER_SHARED_EXPORT PromoteToCustomWidgetCommand : public QDesignerFormWindowCommand
{
public:
    typedef QList<QPointer<QWidget> > WidgetList;

    explicit PromoteToCustomWidgetCommand(QDesignerFormWindowInterface *formWindow);

    void init(const WidgetList &widgets, const QString &customClassName);
    virtual void redo();
    virtual void undo();

private:
    void updateSelection();
    WidgetList m_widgets;
    QString m_customClassName;
};

class QDESIGNER_SHARED_EXPORT DemoteFromCustomWidgetCommand : public QDesignerFormWindowCommand
{
public:
    typedef PromoteToCustomWidgetCommand::WidgetList WidgetList;

    explicit DemoteFromCustomWidgetCommand(QDesignerFormWindowInterface *formWindow);

    void init(const WidgetList &promoted);
    virtual void redo();
    virtual void undo();
private:
    PromoteToCustomWidgetCommand m_promote_cmd;
};

class QDESIGNER_SHARED_EXPORT LayoutCommand: public QDesignerFormWindowCommand
{

public:
    explicit LayoutCommand(QDesignerFormWindowInterface *formWindow);
    virtual ~LayoutCommand();

    inline QList<QWidget*> widgets() const
    { return m_widgets; }

    void init(QWidget *parentWidget, const QList<QWidget*> &widgets, LayoutInfo::Type layoutType,
        QWidget *layoutBase = 0);

    virtual void redo();
    virtual void undo();

private:
    QPointer<QWidget> m_parentWidget;
    QList<QWidget*> m_widgets;
    QPointer<QWidget> m_layoutBase;
    QPointer<Layout> m_layout;
};

class QDESIGNER_SHARED_EXPORT BreakLayoutCommand: public QDesignerFormWindowCommand
{

public:
    explicit BreakLayoutCommand(QDesignerFormWindowInterface *formWindow);
    virtual ~BreakLayoutCommand();

    inline QList<QWidget*> widgets() const
    { return m_widgets; }

    void init(const QList<QWidget*> &widgets, QWidget *layoutBase);

    virtual void redo();
    virtual void undo();

private:
    QList<QWidget*> m_widgets;
    QPointer<QWidget> m_layoutBase;
    QPointer<Layout> m_layout;
    LayoutHelper* m_layoutHelper;
    LayoutProperties *m_properties;
    int m_propertyMask;
};

class QDESIGNER_SHARED_EXPORT SimplifyLayoutCommand: public QDesignerFormWindowCommand
{
public:
    explicit SimplifyLayoutCommand(QDesignerFormWindowInterface *formWindow);
    virtual ~SimplifyLayoutCommand();

    bool init(QWidget *layoutBase);

    // Quick check
    static bool canSimplify(QDesignerFormEditorInterface *core, const QWidget *w, int *layoutType = 0);

    virtual void redo();
    virtual void undo();

private:
    const QRect m_area;
    QWidget *m_layoutBase;
    LayoutHelper* m_layoutHelper;
    bool m_layoutSimplified;
};

class QDESIGNER_SHARED_EXPORT ToolBoxCommand: public QDesignerFormWindowCommand
{

public:
    explicit ToolBoxCommand(QDesignerFormWindowInterface *formWindow);
    virtual ~ToolBoxCommand();

    void init(QToolBox *toolBox);

    virtual void removePage();
    virtual void addPage();

protected:
    QPointer<QToolBox> m_toolBox;
    QPointer<QWidget> m_widget;
    int m_index;
    QString m_itemText;
    QIcon m_itemIcon;
};

class QDESIGNER_SHARED_EXPORT MoveToolBoxPageCommand: public ToolBoxCommand
{

public:
    explicit MoveToolBoxPageCommand(QDesignerFormWindowInterface *formWindow);
    virtual ~MoveToolBoxPageCommand();

    void init(QToolBox *toolBox, QWidget *page, int newIndex);

    virtual void redo();
    virtual void undo();

private:
    int m_newIndex;
    int m_oldIndex;
};

class QDESIGNER_SHARED_EXPORT DeleteToolBoxPageCommand: public ToolBoxCommand
{

public:
    explicit DeleteToolBoxPageCommand(QDesignerFormWindowInterface *formWindow);
    virtual ~DeleteToolBoxPageCommand();

    void init(QToolBox *toolBox);

    virtual void redo();
    virtual void undo();
};

class QDESIGNER_SHARED_EXPORT AddToolBoxPageCommand: public ToolBoxCommand
{

public:
    enum InsertionMode {
        InsertBefore,
        InsertAfter
    };
    explicit AddToolBoxPageCommand(QDesignerFormWindowInterface *formWindow);
    virtual ~AddToolBoxPageCommand();

    void init(QToolBox *toolBox);
    void init(QToolBox *toolBox, InsertionMode mode);

    virtual void redo();
    virtual void undo();
};

class QDESIGNER_SHARED_EXPORT TabWidgetCommand: public QDesignerFormWindowCommand
{

public:
    explicit TabWidgetCommand(QDesignerFormWindowInterface *formWindow);
    virtual ~TabWidgetCommand();

    void init(QTabWidget *tabWidget);

    virtual void removePage();
    virtual void addPage();

protected:
    QPointer<QTabWidget> m_tabWidget;
    QPointer<QWidget> m_widget;
    int m_index;
    QString m_itemText;
    QIcon m_itemIcon;
};

class QDESIGNER_SHARED_EXPORT DeleteTabPageCommand: public TabWidgetCommand
{

public:
    explicit DeleteTabPageCommand(QDesignerFormWindowInterface *formWindow);
    virtual ~DeleteTabPageCommand();

    void init(QTabWidget *tabWidget);

    virtual void redo();
    virtual void undo();
};

class QDESIGNER_SHARED_EXPORT AddTabPageCommand: public TabWidgetCommand
{

public:
    enum InsertionMode {
        InsertBefore,
        InsertAfter
    };
    explicit AddTabPageCommand(QDesignerFormWindowInterface *formWindow);
    virtual ~AddTabPageCommand();

    void init(QTabWidget *tabWidget);
    void init(QTabWidget *tabWidget, InsertionMode mode);

    virtual void redo();
    virtual void undo();
};

class QDESIGNER_SHARED_EXPORT MoveTabPageCommand: public TabWidgetCommand
{

public:
    explicit MoveTabPageCommand(QDesignerFormWindowInterface *formWindow);
    virtual ~MoveTabPageCommand();

    void init(QTabWidget *tabWidget, QWidget *page,
                      const QIcon &icon, const QString &label,
                      int index, int newIndex);

    virtual void redo();
    virtual void undo();

private:
    int m_newIndex;
    int m_oldIndex;
    QPointer<QWidget> m_page;
    QString m_label;
    QIcon m_icon;
};

class QDESIGNER_SHARED_EXPORT StackedWidgetCommand: public QDesignerFormWindowCommand
{

public:
    explicit StackedWidgetCommand(QDesignerFormWindowInterface *formWindow);
    virtual ~StackedWidgetCommand();

    void init(QStackedWidget *stackedWidget);

    virtual void removePage();
    virtual void addPage();

protected:
    QPointer<QStackedWidget> m_stackedWidget;
    QPointer<QWidget> m_widget;
    int m_index;
};

class QDESIGNER_SHARED_EXPORT MoveStackedWidgetCommand: public StackedWidgetCommand
{

public:
    explicit MoveStackedWidgetCommand(QDesignerFormWindowInterface *formWindow);
    virtual ~MoveStackedWidgetCommand();

    void init(QStackedWidget *stackedWidget, QWidget *page, int newIndex);

    virtual void redo();
    virtual void undo();

private:
    int m_newIndex;
    int m_oldIndex;
};

class QDESIGNER_SHARED_EXPORT DeleteStackedWidgetPageCommand: public StackedWidgetCommand
{

public:
    explicit DeleteStackedWidgetPageCommand(QDesignerFormWindowInterface *formWindow);
    virtual ~DeleteStackedWidgetPageCommand();

    void init(QStackedWidget *stackedWidget);

    virtual void redo();
    virtual void undo();
};

class QDESIGNER_SHARED_EXPORT AddStackedWidgetPageCommand: public StackedWidgetCommand
{

public:
    enum InsertionMode {
        InsertBefore,
        InsertAfter
    };
    explicit AddStackedWidgetPageCommand(QDesignerFormWindowInterface *formWindow);
    virtual ~AddStackedWidgetPageCommand();

    void init(QStackedWidget *stackedWidget);
    void init(QStackedWidget *stackedWidget, InsertionMode mode);

    virtual void redo();
    virtual void undo();
};

class QDESIGNER_SHARED_EXPORT CreateMenuBarCommand: public QDesignerFormWindowCommand
{

public:
    explicit CreateMenuBarCommand(QDesignerFormWindowInterface *formWindow);

    void init(QMainWindow *mainWindow);

    virtual void undo();
    virtual void redo();

private:
    QPointer<QMainWindow> m_mainWindow;
    QPointer<QMenuBar> m_menuBar;
};

class QDESIGNER_SHARED_EXPORT DeleteMenuBarCommand: public QDesignerFormWindowCommand
{

public:
    explicit DeleteMenuBarCommand(QDesignerFormWindowInterface *formWindow);

    void init(QMenuBar *menuBar);

    virtual void undo();
    virtual void redo();

private:
    QPointer<QMainWindow> m_mainWindow;
    QPointer<QMenuBar> m_menuBar;
};

class QDESIGNER_SHARED_EXPORT CreateStatusBarCommand: public QDesignerFormWindowCommand
{

public:
    explicit CreateStatusBarCommand(QDesignerFormWindowInterface *formWindow);

    void init(QMainWindow *mainWindow);

    virtual void undo();
    virtual void redo();

private:
    QPointer<QMainWindow> m_mainWindow;
    QPointer<QStatusBar> m_statusBar;
};

class QDESIGNER_SHARED_EXPORT DeleteStatusBarCommand: public QDesignerFormWindowCommand
{

public:
    explicit DeleteStatusBarCommand(QDesignerFormWindowInterface *formWindow);

    void init(QStatusBar *statusBar);

    virtual void undo();
    virtual void redo();

private:
    QPointer<QMainWindow> m_mainWindow;
    QPointer<QStatusBar> m_statusBar;
};

class QDESIGNER_SHARED_EXPORT AddToolBarCommand: public QDesignerFormWindowCommand
{

public:
    explicit AddToolBarCommand(QDesignerFormWindowInterface *formWindow);

    void init(QMainWindow *mainWindow);

    virtual void undo();
    virtual void redo();

private:
    QPointer<QMainWindow> m_mainWindow;
    QPointer<QToolBar> m_toolBar;
};

class QDESIGNER_SHARED_EXPORT DeleteToolBarCommand: public QDesignerFormWindowCommand
{

public:
    explicit DeleteToolBarCommand(QDesignerFormWindowInterface *formWindow);

    void init(QToolBar *toolBar);

    virtual void undo();
    virtual void redo();

private:
    QPointer<QMainWindow> m_mainWindow;
    QPointer<QToolBar> m_toolBar;
};

class QDESIGNER_SHARED_EXPORT DockWidgetCommand: public QDesignerFormWindowCommand
{

public:
    explicit DockWidgetCommand(const QString &description, QDesignerFormWindowInterface *formWindow);
    virtual ~DockWidgetCommand();

    void init(QDockWidget *dockWidget);

protected:
    QPointer<QDockWidget> m_dockWidget;
};

class QDESIGNER_SHARED_EXPORT SetDockWidgetCommand: public DockWidgetCommand
{

public:
    explicit SetDockWidgetCommand(QDesignerFormWindowInterface *formWindow);

    void init(QDockWidget *dockWidget, QWidget *widget);

    virtual void undo();
    virtual void redo();

private:
    QPointer<QWidget> m_widget;
    QPointer<QWidget> m_oldWidget;
};

class QDESIGNER_SHARED_EXPORT AddDockWidgetCommand: public QDesignerFormWindowCommand
{

public:
    explicit AddDockWidgetCommand(QDesignerFormWindowInterface *formWindow);

    void init(QMainWindow *mainWindow, QDockWidget *dockWidget);
    void init(QMainWindow *mainWindow);

    virtual void undo();
    virtual void redo();

private:
    QPointer<QMainWindow> m_mainWindow;
    QPointer<QDockWidget> m_dockWidget;
};

class QDESIGNER_SHARED_EXPORT ContainerWidgetCommand: public QDesignerFormWindowCommand
{

public:
    explicit ContainerWidgetCommand(QDesignerFormWindowInterface *formWindow);
    virtual ~ContainerWidgetCommand();

    QDesignerContainerExtension *containerExtension() const;

    void init(QWidget *containerWidget);

    virtual void removePage();
    virtual void addPage();

protected:
    QPointer<QWidget> m_containerWidget;
    QPointer<QWidget> m_widget;
    int m_index;
};

class QDESIGNER_SHARED_EXPORT DeleteContainerWidgetPageCommand: public ContainerWidgetCommand
{

public:
    explicit DeleteContainerWidgetPageCommand(QDesignerFormWindowInterface *formWindow);
    virtual ~DeleteContainerWidgetPageCommand();

    void init(QWidget *containerWidget, ContainerType ct);

    virtual void redo();
    virtual void undo();
};

class QDESIGNER_SHARED_EXPORT AddContainerWidgetPageCommand: public ContainerWidgetCommand
{

public:
    enum InsertionMode {
        InsertBefore,
        InsertAfter
    };
    explicit AddContainerWidgetPageCommand(QDesignerFormWindowInterface *formWindow);
    virtual ~AddContainerWidgetPageCommand();

    void init(QWidget *containerWidget, ContainerType ct, InsertionMode mode);

    virtual void redo();
    virtual void undo();
};

// Data structure representing the contents of a QTableWidget with
// methods to retrieve and apply for ChangeTableContentsCommand
struct QDESIGNER_SHARED_EXPORT TableWidgetContents {
    // Cell data (icon, text). Needs comparison
    struct CellData {
        CellData();
        CellData(const QString &text, const PropertySheetIconValue &icon);

        QTableWidgetItem *createItem(DesignerIconCache *iconCache) const;

        bool equals(const CellData &rhs) const;
        bool operator==(const CellData &rhs) const { return equals(rhs); }
        bool operator!=(const CellData &rhs) const { return !equals(rhs); }

        QString m_text;
        PropertySheetIconValue m_icon;
    };

    typedef QPair<int, int> CellRowColumnAddress;
    typedef QMap<int, CellData> Header;
    typedef QMap<CellRowColumnAddress, CellData> TableItemMap;

    TableWidgetContents();
    void clear();

    void fromTableWidget(const QTableWidget *tableWidget);
    void applyToTableWidget(QTableWidget *tableWidget, DesignerIconCache *iconCache) const;

    bool equals(const TableWidgetContents &rhs) const;
    bool operator==(const TableWidgetContents &rhs) const { return equals(rhs); }
    bool operator!=(const TableWidgetContents &rhs) const { return !equals(rhs); }

    static QString defaultHeaderText(int i);
    static void insertHeaderItem(const QTableWidgetItem *item, int i, Header *header);

    int m_columnCount;
    int m_rowCount;
    Header m_horizontalHeader;
    Header m_verticalHeader;
    TableItemMap m_items;
};

class QDESIGNER_SHARED_EXPORT ChangeTableContentsCommand: public QDesignerFormWindowCommand
{
public:
    explicit ChangeTableContentsCommand(QDesignerFormWindowInterface *formWindow);

    // returns false widgets have the same contents
    bool init(QTableWidget *tableWidget, QTableWidget *fromTableWidget);
    virtual void redo();
    virtual void undo();

private:
    QPointer<QTableWidget> m_tableWidget;
    TableWidgetContents m_oldContents;
    TableWidgetContents m_newContents;
    DesignerIconCache *m_iconCache;
};

class QDESIGNER_SHARED_EXPORT ChangeTreeContentsCommand: public QDesignerFormWindowCommand
{

public:
    explicit ChangeTreeContentsCommand(QDesignerFormWindowInterface *formWindow);
    ~ChangeTreeContentsCommand();

    void init(QTreeWidget *treeWidget, QTreeWidget *fromTreeWidget);
    virtual void redo();
    virtual void undo();
    enum ApplyIconStrategy {
        SetIconStrategy,
        ResetIconStrategy
    };
private:
    void applyIcon(QTreeWidgetItem *item, ChangeTreeContentsCommand::ApplyIconStrategy strategy) const;
    QTreeWidgetItem *cloneItem(QTreeWidgetItem *origItem, ChangeTreeContentsCommand::ApplyIconStrategy strategy) const;
    void initState(QList<QTreeWidgetItem *> &itemsState,
                QTreeWidgetItem *&headerItemState, QTreeWidget *fromTreeWidget) const;
    void changeContents(QTreeWidget *treeWidget, int columnCount,
                const QList<QTreeWidgetItem *> &itemsState,
                QTreeWidgetItem *headerItemState) const;
    void clearState(QList<QTreeWidgetItem *> &itemsState,
                QTreeWidgetItem *&headerItemState) const;

    QPointer<QTreeWidget> m_treeWidget;
    QTreeWidgetItem *m_oldHeaderItemState;
    QTreeWidgetItem *m_newHeaderItemState;
    QList<QTreeWidgetItem *> m_oldItemsState;
    QList<QTreeWidgetItem *> m_newItemsState;
    int m_oldColumnCount;
    int m_newColumnCount;
    DesignerIconCache *m_iconCache;
};

class QDESIGNER_SHARED_EXPORT ChangeListContentsCommand: public QDesignerFormWindowCommand
{

public:
    explicit ChangeListContentsCommand(QDesignerFormWindowInterface *formWindow);

    void init(QListWidget *listWidget, const QList<QPair<QString, PropertySheetIconValue> > &items);
    void init(QComboBox *comboBox, const QList<QPair<QString, PropertySheetIconValue> > &items);
    virtual void redo();
    virtual void undo();
private:
    void changeContents(QListWidget *listWidget,
        const QList<QPair<QString, PropertySheetIconValue> > &itemsState) const;
    void changeContents(QComboBox *comboBox,
        const QList<QPair<QString, PropertySheetIconValue> > &itemsState) const;

    QPointer<QListWidget> m_listWidget;
    QPointer<QComboBox> m_comboBox;
    QList<QPair<QString, PropertySheetIconValue> > m_oldItemsState;
    QList<QPair<QString, PropertySheetIconValue> > m_newItemsState;
    DesignerIconCache *m_iconCache;
};

class QDESIGNER_SHARED_EXPORT AddActionCommand : public QDesignerFormWindowCommand
{

public:
    explicit AddActionCommand(QDesignerFormWindowInterface *formWindow);
    void init(QAction *action);
    virtual void redo();
    virtual void undo();
private:
    QAction *m_action;
};

class QDESIGNER_SHARED_EXPORT RemoveActionCommand : public QDesignerFormWindowCommand
{

public:
    explicit RemoveActionCommand(QDesignerFormWindowInterface *formWindow);
    void init(QAction *action);
    virtual void redo();
    virtual void undo();

    struct ActionDataItem {
        ActionDataItem(QAction *_before = 0, QWidget *_widget = 0)
            : before(_before), widget(_widget) {}
        QAction *before;
        QWidget *widget;
    };
    typedef QList<ActionDataItem> ActionData;

private:
    QAction *m_action;

    ActionData m_actionData;
};

class QDESIGNER_SHARED_EXPORT ActionInsertionCommand : public QDesignerFormWindowCommand
{

protected:
    ActionInsertionCommand(const QString &text, QDesignerFormWindowInterface *formWindow);

public:
    void init(QWidget *parentWidget, QAction *action, QAction *beforeAction = 0, bool update = true);

protected:
    void insertAction();
    void removeAction();

private:
    QWidget *m_parentWidget;
    QAction *m_action;
    QAction *m_beforeAction;
    bool m_update;
};

class QDESIGNER_SHARED_EXPORT InsertActionIntoCommand : public ActionInsertionCommand
{

public:
    explicit InsertActionIntoCommand(QDesignerFormWindowInterface *formWindow);

    virtual void redo() {  insertAction(); }
    virtual void undo() {  removeAction(); }
};

class QDESIGNER_SHARED_EXPORT RemoveActionFromCommand : public ActionInsertionCommand
{

public:
    explicit RemoveActionFromCommand(QDesignerFormWindowInterface *formWindow);

    virtual void redo()  {  removeAction(); }
    virtual void undo()  {  insertAction(); }
};

class QDESIGNER_SHARED_EXPORT MenuActionCommand : public QDesignerFormWindowCommand
{
public:
    void init(QAction *action, QAction *actionBefore, QWidget *associatedWidget, QWidget *objectToSelect);

protected:
    MenuActionCommand(const QString &text, QDesignerFormWindowInterface *formWindow);
    void insertMenu();
    void removeMenu();

private:
    QAction *m_action;
    QAction *m_actionBefore;
    QWidget *m_menuParent;
    QWidget *m_associatedWidget;
    QWidget *m_objectToSelect;
};

class QDESIGNER_SHARED_EXPORT AddMenuActionCommand : public MenuActionCommand
{

public:
    explicit AddMenuActionCommand(QDesignerFormWindowInterface *formWindow);

    virtual void redo() { insertMenu(); }
    virtual void undo() { removeMenu(); }
};

class QDESIGNER_SHARED_EXPORT RemoveMenuActionCommand : public MenuActionCommand
{

public:
    explicit RemoveMenuActionCommand(QDesignerFormWindowInterface *formWindow);

    virtual void redo() { removeMenu(); }
    virtual void undo() { insertMenu(); }
};

class QDESIGNER_SHARED_EXPORT CreateSubmenuCommand : public QDesignerFormWindowCommand
{

public:
    explicit CreateSubmenuCommand(QDesignerFormWindowInterface *formWindow);
    void init(QDesignerMenu *menu, QAction *action, QObject *m_objectToSelect = 0);
    virtual void redo();
    virtual void undo();
private:
    QAction *m_action;
    QDesignerMenu *m_menu;
    QObject *m_objectToSelect;
};

} // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // QDESIGNER_COMMAND_H
