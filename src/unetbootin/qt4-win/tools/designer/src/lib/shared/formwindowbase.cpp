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

/*
TRANSLATOR qdesigner_internal::FormWindowBase
*/

#include "formwindowbase_p.h"
#include "connectionedit_p.h"
#include "qdesigner_command_p.h"
#include "qdesigner_propertysheet_p.h"
#include "qdesigner_propertyeditor_p.h"
#include <abstractformbuilder.h>

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerContainerExtension>
#include <QtDesigner/QExtensionManager>

#include <QtCore/qdebug.h>
#include <QtGui/QMenu>
#include <QtGui/QListWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QComboBox>
#include <QtGui/QTabWidget>
#include <QtGui/QToolBox>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

QPixmap DesignerPixmapCache::pixmap(const PropertySheetPixmapValue &value) const
{
    QMap<PropertySheetPixmapValue, QPixmap>::const_iterator it = m_cache.constFind(value);
    if (it != m_cache.constEnd())
        return it.value();

    QPixmap pix = QPixmap(value.path());
    m_cache.insert(value, pix);
    return pix;
}

void DesignerPixmapCache::clear()
{
    m_cache.clear();
}

DesignerPixmapCache::DesignerPixmapCache(QObject *parent)
    : QObject(parent)
{
}

QIcon DesignerIconCache::icon(const PropertySheetIconValue &value) const
{
    QMap<PropertySheetIconValue, QIcon>::const_iterator it = m_cache.constFind(value);
    if (it != m_cache.constEnd())
        return it.value();

    QIcon icon;
    QMap<QPair<QIcon::Mode, QIcon::State>, PropertySheetPixmapValue> paths = value.paths();
    QMapIterator<QPair<QIcon::Mode, QIcon::State>, PropertySheetPixmapValue> itPath(paths);
    while (itPath.hasNext()) {
        QPair<QIcon::Mode, QIcon::State> pair = itPath.next().key();
        icon.addPixmap(m_pixmapCache->pixmap(itPath.value()), pair.first, pair.second);
    }
    m_cache.insert(value, icon);
    return icon;
}

void DesignerIconCache::clear()
{
    m_cache.clear();
}

DesignerIconCache::DesignerIconCache(DesignerPixmapCache *pixmapCache, QObject *parent)
    : QObject(parent),
      m_pixmapCache(pixmapCache)
{

}

Grid FormWindowBase::m_defaultGrid;

FormWindowBase::FormWindowBase(QWidget *parent, Qt::WindowFlags flags) :
    QDesignerFormWindowInterface(parent, flags),
    m_feature(DefaultFeature),
    m_grid(m_defaultGrid),
    m_hasFormGrid(false),
    m_resourceSet(0)
{
    syncGridFeature();
    m_pixmapCache = new DesignerPixmapCache(this);
    m_iconCache = new DesignerIconCache(m_pixmapCache, this);
}

DesignerPixmapCache *FormWindowBase::pixmapCache() const
{
    return m_pixmapCache;
}

DesignerIconCache *FormWindowBase::iconCache() const
{
    return m_iconCache;
}

QtResourceSet *FormWindowBase::resourceSet() const
{
    return m_resourceSet;
}

void FormWindowBase::setResourceSet(QtResourceSet *resourceSet)
{
    m_resourceSet = resourceSet;
}

void FormWindowBase::addReloadableProperty(QDesignerPropertySheet *sheet, int index)
{
    m_reloadableResources[sheet][index] = true;
}

void FormWindowBase::removeReloadableProperty(QDesignerPropertySheet *sheet, int index)
{
    m_reloadableResources[sheet].remove(index);
    if (m_reloadableResources[sheet].count() == 0)
        m_reloadableResources.remove(sheet);
}

void FormWindowBase::addReloadablePropertySheet(QDesignerPropertySheet *sheet, QObject *object)
{
    if (qobject_cast<QTreeWidget *>(object) ||
            qobject_cast<QTableWidget *>(object) ||
            qobject_cast<QListWidget *>(object) ||
            qobject_cast<QComboBox *>(object))
        m_reloadablePropertySheets[sheet] = object;
}

void FormWindowBase::removeReloadablePropertySheet(QDesignerPropertySheet *sheet)
{
    m_reloadablePropertySheets.remove(sheet);
}

void FormWindowBase::reloadProperties()
{
    pixmapCache()->clear();
    iconCache()->clear();
    QMapIterator<QDesignerPropertySheet *, QMap<int, bool> > itSheet(m_reloadableResources);
    while (itSheet.hasNext()) {
        QDesignerPropertySheet *sheet = itSheet.next().key();
        QMapIterator<int, bool> itIndex(itSheet.value());
        while (itIndex.hasNext()) {
            const int index = itIndex.next().key();
            sheet->setProperty(index, sheet->property(index));
        }
        if (QTabWidget *tabWidget = qobject_cast<QTabWidget *>(sheet->object())) {
            const int count = tabWidget->count();
            const int current = tabWidget->currentIndex();
            const QString currentTabIcon = QLatin1String("currentTabIcon");
            for (int i = 0; i < count; i++) {
                tabWidget->setCurrentIndex(i);
                const int index = sheet->indexOf(currentTabIcon);
                sheet->setProperty(index, sheet->property(index));
            }
            tabWidget->setCurrentIndex(current);
        } else if (QToolBox *toolBox = qobject_cast<QToolBox *>(sheet->object())) {
            const int count = toolBox->count();
            const int current = toolBox->currentIndex();
            const QString currentItemIcon = QLatin1String("currentItemIcon");
            for (int i = 0; i < count; i++) {
                toolBox->setCurrentIndex(i);
                const int index = sheet->indexOf(currentItemIcon);
                sheet->setProperty(index, sheet->property(index));
            }
            toolBox->setCurrentIndex(current);
        }
    }
    QMapIterator<QDesignerPropertySheet *, QObject *> itSh(m_reloadablePropertySheets);
    while (itSh.hasNext()) {
        QObject *object = itSh.next().value();
        reloadIconResources(iconCache(), object);
    }
}

void FormWindowBase::resourceSetActivated(QtResourceSet *resource, bool resourceSetChanged)
{
    if (resource == resourceSet() && resourceSetChanged) {
        reloadProperties();
        emit pixmapCache()->reloaded();
        emit iconCache()->reloaded();
        if (QDesignerPropertyEditor *propertyEditor = qobject_cast<QDesignerPropertyEditor *>(core()->propertyEditor()))
            propertyEditor->reloadResourceProperties();
    }
}

QVariantMap FormWindowBase::formData()
{
    QVariantMap rc;
    if (m_hasFormGrid)
        m_grid.addToVariantMap(rc, true);
    return rc;
}

void FormWindowBase::setFormData(const QVariantMap &vm)
{
    Grid formGrid;
    m_hasFormGrid = formGrid.fromVariantMap(vm);
    if (m_hasFormGrid)
         m_grid = formGrid;
}

QPoint FormWindowBase::grid() const
{
    return QPoint(m_grid.deltaX(), m_grid.deltaY());
}

void FormWindowBase::setGrid(const QPoint &grid)
{
    m_grid.setDeltaX(grid.x());
    m_grid.setDeltaY(grid.y());
}

bool FormWindowBase::hasFeature(Feature f) const
{
    return f & m_feature;
}

static void recursiveUpdate(QWidget *w)
{
    w->update();

    const QObjectList &l = w->children();
    const QObjectList::const_iterator cend = l.constEnd();
    for (QObjectList::const_iterator it = l.constBegin(); it != cend; ++it) {
        if (QWidget *w = qobject_cast<QWidget*>(*it))
            recursiveUpdate(w);
    }
}

void FormWindowBase::setFeatures(Feature f)
{
    m_feature = f;
    const bool enableGrid = f & GridFeature;
    m_grid.setVisible(enableGrid);
    m_grid.setSnapX(enableGrid);
    m_grid.setSnapY(enableGrid);
    emit featureChanged(f);
    recursiveUpdate(this);
}

FormWindowBase::Feature FormWindowBase::features() const
{
    return m_feature;
}

bool FormWindowBase::gridVisible() const
{
    return m_grid.visible() && currentTool() == 0;
}

void FormWindowBase::syncGridFeature()
{
    if (m_grid.snapX() || m_grid.snapY())
        m_feature |= GridFeature;
    else
        m_feature &= ~GridFeature;
}

void FormWindowBase::setDesignerGrid(const  Grid& grid)
{
    m_grid = grid;
    syncGridFeature();
    recursiveUpdate(this);
}

void FormWindowBase::setDefaultDesignerGrid(const  Grid& grid)
{
    m_defaultGrid = grid;
}

QMenu *FormWindowBase::initializePopupMenu(QWidget * /*managedWidget*/)
{
    return 0;
}

// Widget under mouse for finding the Widget to highlight
// when doing DnD. Restricts to pages by geometry if a container with
// a container extension (or one of its helper widgets) is hit; otherwise
// returns the widget as such (be it managed/unmanaged)

QWidget *FormWindowBase::widgetUnderMouse(const QPoint &formPos, WidgetUnderMouseMode /* wum */)
{
    // widget_under_mouse might be some temporary thing like the dropLine. We need
    // the actual widget that's part of the edited GUI.
    QWidget *rc = widgetAt(formPos);
    if (!rc || qobject_cast<ConnectionEdit*>(rc))
        return 0;

    if (rc == mainContainer()) {
        // Refuse main container areas if the main container has a container extension,
        // for example when hitting QToolBox/QTabWidget empty areas.
        if (qt_extension<QDesignerContainerExtension*>(core()->extensionManager(), rc))
            return 0;
        return rc;
    }

    // If we hit on container extension type container, make sure
    // we use the top-most current page
    if (QWidget *container = findContainer(rc, false))
        if (QDesignerContainerExtension *c = qt_extension<QDesignerContainerExtension*>(core()->extensionManager(), container)) {
            // For container that do not have a "stacked" nature (QToolBox, QMdiArea),
            // make sure the position is within the current page
            const int ci = c->currentIndex();
            if (ci < 0)
                return 0;
            QWidget *page = c->widget(ci);
            QRect pageGeometry = page->geometry();
            pageGeometry.moveTo(page->mapTo(this, pageGeometry.topLeft()));
            if (!pageGeometry.contains(formPos))
                return 0;
            return page;
        }

    return rc;
}

void FormWindowBase::deleteWidgetList(const QWidgetList &widget_list)
{
    switch (widget_list.size()) {
    case 0:
        break;
    case 1: {
        commandHistory()->beginMacro(tr("Delete '%1'").arg(widget_list.front()->objectName()));
        emit widgetRemoved(widget_list.front());
        DeleteWidgetCommand *cmd = new DeleteWidgetCommand(this);
        cmd->init(widget_list.front());
        commandHistory()->push(cmd);
        commandHistory()->endMacro();
    }
        break;
    default:
        commandHistory()->beginMacro(tr("Delete"));
        foreach (QWidget *w, widget_list) {
            emit widgetRemoved(w);
            DeleteWidgetCommand *cmd = new DeleteWidgetCommand(this);
            cmd->init(w);
            commandHistory()->push(cmd);
        }
        commandHistory()->endMacro();
        break;
    }
}

} // namespace qdesigner_internal

QT_END_NAMESPACE
