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

#ifndef FORMWINDOWBASE_H
#define FORMWINDOWBASE_H

#include "shared_global_p.h"
#include "qdesigner_utils_p.h"
#include "grid_p.h"

#include <QtDesigner/QDesignerFormWindowInterface>

#include <QtCore/QVariantMap>
#include <QtCore/QList>

QT_BEGIN_NAMESPACE

class QDesignerDnDItemInterface;
class QMenu;
class QListWidgetItem;
class QTreeWidgetItem;
class QTableWidgetItem;
class QPoint;
class QtResourceSet;
class QDesignerPropertySheet;

namespace qdesigner_internal {

class QEditorFormBuilder;

class QDESIGNER_SHARED_EXPORT DesignerPixmapCache : public QObject
{
    Q_OBJECT
public:
    DesignerPixmapCache(QObject *parent = 0);
    QPixmap pixmap(const PropertySheetPixmapValue &value) const;
    void clear();
signals:
    void reloaded();
private:
    mutable QMap<PropertySheetPixmapValue, QPixmap> m_cache;
    friend class FormWindowBase;
};

class QDESIGNER_SHARED_EXPORT DesignerIconCache : public QObject
{
    Q_OBJECT
public:
    DesignerIconCache(DesignerPixmapCache *pixmapCache, QObject *parent = 0);
    QIcon icon(const PropertySheetIconValue &value) const;
    void clear();
signals:
    void reloaded();
private:
    mutable QMap<PropertySheetIconValue, QIcon> m_cache;
    DesignerPixmapCache *m_pixmapCache;
    friend class FormWindowBase;
};

class QDESIGNER_SHARED_EXPORT FormWindowBase: public QDesignerFormWindowInterface
{
    Q_OBJECT

public:
    enum HighlightMode  { Restore, Highlight };

    FormWindowBase(QWidget *parent = 0, Qt::WindowFlags flags = 0);

    QVariantMap formData();
    void setFormData(const QVariantMap &vm);

    // Deprecated
    virtual QPoint grid() const;

    // Deprecated
    virtual void setGrid(const QPoint &grid);

    virtual bool hasFeature(Feature f) const;
    virtual Feature features() const;
    virtual void setFeatures(Feature f);

    const Grid &designerGrid() const { return m_grid; }
    void setDesignerGrid(const  Grid& grid);

    bool hasFormGrid() const { return m_hasFormGrid; }
    void setHasFormGrid(bool b) { m_hasFormGrid = b; }

    bool gridVisible() const;

    static const Grid &defaultDesignerGrid() { return m_defaultGrid; }
    static void setDefaultDesignerGrid(const  Grid& grid);

    // Overwrite to initialize and return a popup menu for a managed widget
    virtual QMenu *initializePopupMenu(QWidget *managedWidget);

    virtual bool dropWidgets(const QList<QDesignerDnDItemInterface*> &item_list, QWidget *target,
                             const QPoint &global_mouse_pos) = 0;

    // Helper to find the widget at the mouse position with some flags.
    enum WidgetUnderMouseMode { FindSingleSelectionDropTarget, FindMultiSelectionDropTarget };
    QWidget *widgetUnderMouse(const QPoint &formPos, WidgetUnderMouseMode m);

    virtual QWidget *widgetAt(const QPoint &pos) = 0;
    virtual QWidget *findContainer(QWidget *w, bool excludeLayout) const = 0;

    void deleteWidgetList(const QWidgetList &widget_list);

    virtual void highlightWidget(QWidget *w, const QPoint &pos, HighlightMode mode = Highlight) = 0;

    enum PasteMode { PasteAll, PasteActionsOnly };
    virtual void paste(PasteMode pasteMode) = 0;

    // Factory method to create a form builder
    virtual QEditorFormBuilder *createFormBuilder() = 0;

    virtual bool blockSelectionChanged(bool blocked) = 0;
    virtual void emitSelectionChanged() = 0;

    DesignerPixmapCache *pixmapCache() const;
    DesignerIconCache *iconCache() const;
    QtResourceSet *resourceSet() const;
    void setResourceSet(QtResourceSet *resourceSet);
    void addReloadableProperty(QDesignerPropertySheet *sheet, int index);
    void removeReloadableProperty(QDesignerPropertySheet *sheet, int index);
    void addReloadablePropertySheet(QDesignerPropertySheet *sheet, QObject *object);
    void removeReloadablePropertySheet(QDesignerPropertySheet *sheet);
    void reloadProperties();

public slots:

    void resourceSetActivated(QtResourceSet *resourceSet, bool resourceSetChanged);

private:
    void syncGridFeature();
    static Grid m_defaultGrid;

    Feature m_feature;
    Grid m_grid;
    bool m_hasFormGrid;
    DesignerPixmapCache *m_pixmapCache;
    DesignerIconCache *m_iconCache;
    QtResourceSet *m_resourceSet;
    QMap<QDesignerPropertySheet *, QMap<int, bool> > m_reloadableResources; // bool is dummy, QMap used as QSet
    QMap<QDesignerPropertySheet *, QObject *> m_reloadablePropertySheets;
};

}  // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // FORMWINDOWBASE_H
