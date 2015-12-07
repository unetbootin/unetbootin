/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

/*
TRANSLATOR qdesigner_internal::ObjectInspector
*/

#include "objectinspectormodel_p.h"

#include <qlayout_widget_p.h>
#include <layout_p.h>
#include <qdesigner_propertycommand_p.h>
#include <qdesigner_utils_p.h>
#include <iconloader_p.h>

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerWidgetDataBaseInterface>
#include <QtDesigner/QDesignerContainerExtension>
#include <QtDesigner/QDesignerMetaDataBaseInterface>
#include <QtDesigner/QExtensionManager>
#include <QtGui/QLayout>
#include <QtGui/QAction>
#include <QtGui/QLayoutItem>
#include <QtGui/QMenu>
#include <QtGui/QButtonGroup>
#include <QtCore/QSet>
#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>

QT_BEGIN_NAMESPACE

namespace {
    enum { DataRole = 1000 };
}

static inline QObject *objectOfItem(const QStandardItem *item) {
    return qvariant_cast<QObject *>(item->data(DataRole));
}

static bool sortEntry(const QObject *a, const QObject *b)
{
    return a->objectName() < b->objectName();
}

static bool sameIcon(const QIcon &i1, const QIcon &i2)
{
    if (i1.isNull() &&  i2.isNull())
             return true;
    if (i1.isNull() !=  i2.isNull())
        return false;
    return i1.serialNumber() == i2.serialNumber();
}

static inline bool isNameColumnEditable(const QObject *)
{
    return true;
}

static qdesigner_internal::ObjectData::StandardItemList createModelRow(const QObject *o)
{
    qdesigner_internal::ObjectData::StandardItemList rc;
    const Qt::ItemFlags baseFlags = Qt::ItemIsSelectable|Qt::ItemIsDropEnabled|Qt::ItemIsEnabled;
    for (int i = 0; i < qdesigner_internal::ObjectInspectorModel::NumColumns; i++) {
        QStandardItem *item = new QStandardItem;
        Qt::ItemFlags flags = baseFlags;
        if (i == qdesigner_internal::ObjectInspectorModel::ObjectNameColumn && isNameColumnEditable(o))
            flags |= Qt::ItemIsEditable;
        item->setFlags(flags);
        rc += item;
    }
    return rc;
}

static inline bool isQLayoutWidget(const QObject *o)
{
    return o->metaObject() == &QLayoutWidget::staticMetaObject;
}

namespace qdesigner_internal {

    // context kept while building a model, just there to reduce string allocations
    struct ModelRecursionContext {
        explicit ModelRecursionContext(QDesignerFormEditorInterface *core, const QString &sepName);

        const QString designerPrefix;
        const QString separator;

        QDesignerFormEditorInterface *core;
        const QDesignerWidgetDataBaseInterface *db;
        const QDesignerMetaDataBaseInterface *mdb;
    };

    ModelRecursionContext::ModelRecursionContext(QDesignerFormEditorInterface *c, const QString &sepName) :
        designerPrefix(QLatin1String("QDesigner")),
        separator(sepName),
        core(c),
        db(c->widgetDataBase()),
        mdb(c->metaDataBase())
    {
    }

    // ------------  ObjectData/ ObjectModel:
    // Whenever the selection changes, ObjectInspector::setFormWindow is
    // called. To avoid rebuilding the tree every time (loosing expanded state)
    // a model is first built from the object tree by recursion.
    // As a tree is difficult to represent, a flat list of entries (ObjectData)
    // containing object and parent object is used.
    // ObjectData has an overloaded operator== that compares the object pointers.
    // Structural changes which cause a rebuild can be detected by
    // comparing the lists of ObjectData. If it is the same, only the item data (class name [changed by promotion],
    // object name and icon) are checked and the existing items are updated.

    ObjectData::ObjectData() :
        m_parent(0),
        m_object(0),
        m_type(Object),
        m_managedLayoutType(LayoutInfo::NoLayout)
    {
    }

    ObjectData::ObjectData(QObject *parent, QObject *object, const ModelRecursionContext &ctx) :
       m_parent(parent),
       m_object(object),
       m_type(Object),
       m_className(QLatin1String(object->metaObject()->className())),
       m_objectName(object->objectName()),
       m_managedLayoutType(LayoutInfo::NoLayout)
    {

        // 1) set entry
        if (object->isWidgetType()) {
            initWidget(static_cast<QWidget*>(object), ctx);
        } else {
            initObject(ctx);
        }
        if (m_className.startsWith(ctx.designerPrefix))
            m_className.remove(1, ctx.designerPrefix.size() - 1);
    }

    void ObjectData::initObject(const ModelRecursionContext &ctx)
    {
        // Check objects: Action?
        if (const QAction *act = qobject_cast<const QAction*>(m_object)) {
            if (act->isSeparator()) {  // separator is reserved
                m_objectName = ctx.separator;
                m_type = SeparatorAction;
            } else {
                m_type = Action;
            }
            m_classIcon = act->icon();
        } else {
            m_type = Object;
        }
    }

    void ObjectData::initWidget(QWidget *w, const ModelRecursionContext &ctx)
    {
        // Check for extension container, QLayoutwidget, or normal container
        bool isContainer = false;
        if (const QDesignerWidgetDataBaseItemInterface *widgetItem = ctx.db->item(ctx.db->indexOfObject(w, true))) {
            m_classIcon = widgetItem->icon();
            m_className = widgetItem->name();
            isContainer = widgetItem->isContainer();
        }

        if (isQLayoutWidget(w)) {
            m_type = LayoutWidget;
            const QLayout *layout = w->layout();
            m_managedLayoutType = LayoutInfo::layoutType(ctx.core, layout);
            m_className = QLatin1String(layout->metaObject()->className());
            m_objectName = layout->objectName();
            return;
        }

        if (qt_extension<QDesignerContainerExtension*>(ctx.core->extensionManager(), w)) {
            m_type = ExtensionContainer;
            return;
        }
        if (isContainer) {
            m_type = LayoutableContainer;
            m_managedLayoutType = LayoutInfo::managedLayoutType(ctx.core, w);
            return;
        }
        m_type = ChildWidget;
    }

    bool ObjectData::equals(const ObjectData & me) const
    {
        return m_parent == me.m_parent && m_object == me.m_object;
    }

    unsigned ObjectData::compare(const ObjectData & rhs) const
    {
        unsigned rc = 0;
        if (m_className != rhs.m_className)
            rc |= ClassNameChanged;
        if (m_objectName != rhs.m_objectName)
            rc |= ObjectNameChanged;
        if (!sameIcon(m_classIcon, rhs.m_classIcon))
            rc |= ClassIconChanged;
        if (m_type != rhs.m_type)
            rc |= TypeChanged;
        if (m_managedLayoutType != rhs.m_managedLayoutType)
            rc |= LayoutTypeChanged;
        return rc;
    }

    void ObjectData::setItemsDisplayData(const StandardItemList &row, const ObjectInspectorIcons &icons, unsigned mask) const
    {
        if (mask & ObjectNameChanged)
            row[ObjectInspectorModel::ObjectNameColumn]->setText(m_objectName);
        if (mask & ClassNameChanged) {
            row[ObjectInspectorModel::ClassNameColumn]->setText(m_className);
            row[ObjectInspectorModel::ClassNameColumn]->setToolTip(m_className);
        }
        // Set a layout icon only for containers. Note that QLayoutWidget don't have
        // real class icons
        if (mask & (ClassIconChanged|TypeChanged|LayoutTypeChanged)) {
            switch (m_type) {
            case LayoutWidget:
                row[ObjectInspectorModel::ObjectNameColumn]->setIcon(icons.layoutIcons[m_managedLayoutType]);
                row[ObjectInspectorModel::ClassNameColumn]->setIcon(icons.layoutIcons[m_managedLayoutType]);
                break;
            case LayoutableContainer:
                row[ObjectInspectorModel::ObjectNameColumn]->setIcon(icons.layoutIcons[m_managedLayoutType]);
                row[ObjectInspectorModel::ClassNameColumn]->setIcon(m_classIcon);
                break;
            default:
                row[ObjectInspectorModel::ObjectNameColumn]->setIcon(QIcon());
                row[ObjectInspectorModel::ClassNameColumn]->setIcon(m_classIcon);
                break;
            }
        }
    }

    void ObjectData::setItems(const StandardItemList &row, const ObjectInspectorIcons &icons) const
    {
        const QVariant object = qVariantFromValue(m_object);
        row[ObjectInspectorModel::ObjectNameColumn]->setData(object, DataRole);
        row[ObjectInspectorModel::ClassNameColumn]->setData(object, DataRole);
        setItemsDisplayData(row, icons, ClassNameChanged|ObjectNameChanged|ClassIconChanged|TypeChanged|LayoutTypeChanged);
    }

    typedef QList<ObjectData> ObjectModel;

    // Recursive routine that creates the model by traversing the form window object tree.
    void createModelRecursion(const QDesignerFormWindowInterface *fwi,
                              QObject *parent,
                              QObject *object,
                              ObjectModel &model,
                              const ModelRecursionContext &ctx)
    {
        typedef QList<QButtonGroup *> ButtonGroupList;
        typedef QList<QAction *> ActionList;

        // 1) Create entry
        const ObjectData entry(parent, object, ctx);
        model.push_back(entry);

        // 2) recurse over widget children via container extension or children list
        const QDesignerContainerExtension *containerExtension = 0;
        if (entry.type() == ObjectData::ExtensionContainer) {
            containerExtension = qt_extension<QDesignerContainerExtension*>(fwi->core()->extensionManager(), object);
            Q_ASSERT(containerExtension);
            const int count = containerExtension->count();
            for (int i=0; i < count; ++i) {
                QObject *page = containerExtension->widget(i);
                Q_ASSERT(page != 0);
                createModelRecursion(fwi, object, page, model, ctx);
            }
        }

        QObjectList children = object->children();
        if (!children.empty()) {
            ButtonGroupList buttonGroups;
            qSort(children.begin(), children.end(), sortEntry);
            const QObjectList::const_iterator cend = children.constEnd();
            for (QObjectList::const_iterator it = children.constBegin(); it != cend; ++it) {
                // Managed child widgets unless we had a container extension
                if ((*it)->isWidgetType()) {
                    if (!containerExtension) {
                        QWidget *widget = qobject_cast<QWidget*>(*it);
                        if (fwi->isManaged(widget))
                            createModelRecursion(fwi, object, widget, model, ctx);
                    }
                } else {
                    if (ctx.mdb->item(*it)) {
                        if (QButtonGroup *bg = qobject_cast<QButtonGroup*>(*it))
                            buttonGroups.push_back(bg);
                    } // Has MetaDataBase entry
                }
            }
            // Add button groups
            if (!buttonGroups.empty()) {
                const ButtonGroupList::const_iterator bgcend = buttonGroups.constEnd();
                for (ButtonGroupList::const_iterator bgit = buttonGroups.constBegin(); bgit != bgcend; ++bgit)
                    createModelRecursion(fwi, object, *bgit, model, ctx);
            }
        } // has children
        if (object->isWidgetType()) {
            // Add actions
            const ActionList actions = static_cast<QWidget*>(object)->actions();
            if (!actions.empty()) {
                const ActionList::const_iterator cend = actions.constEnd();
                    for (ActionList::const_iterator it = actions.constBegin(); it != cend; ++it)
                    if (ctx.mdb->item(*it)) {
                        QAction *action = *it;
                        QObject *obj = action;
                            if (action->menu())
                            obj = action->menu();
                        createModelRecursion(fwi, object, obj, model, ctx);
                    }
            }
        }
    }

    // ------------ ObjectInspectorModel
    ObjectInspectorModel::ObjectInspectorModel(QObject *parent) :
       QStandardItemModel(0, NumColumns, parent)
    {
        QStringList headers;
        headers += QCoreApplication::translate("ObjectInspectorModel", "Object");
        headers += QCoreApplication::translate("ObjectInspectorModel", "Class");
        Q_ASSERT(headers.size() == NumColumns);
        setColumnCount(NumColumns);
        setHorizontalHeaderLabels(headers);
        // Icons
        m_icons.layoutIcons[LayoutInfo::NoLayout] = createIconSet(QLatin1String("editbreaklayout.png"));
        m_icons.layoutIcons[LayoutInfo::HSplitter] = createIconSet(QLatin1String("edithlayoutsplit.png"));
        m_icons.layoutIcons[LayoutInfo::VSplitter] = createIconSet(QLatin1String("editvlayoutsplit.png"));
        m_icons.layoutIcons[LayoutInfo::HBox] = createIconSet(QLatin1String("edithlayout.png"));
        m_icons.layoutIcons[LayoutInfo::VBox] = createIconSet(QLatin1String("editvlayout.png"));
        m_icons.layoutIcons[LayoutInfo::Grid] = createIconSet(QLatin1String("editgrid.png"));
        m_icons.layoutIcons[LayoutInfo::Form] = createIconSet(QLatin1String("editform.png"));
    }

    void ObjectInspectorModel::clearItems()
    {
        m_objectIndexMultiMap.clear();
        m_model.clear();
        reset(); // force editors to be closed in views
        removeRow(0);
    }

    ObjectInspectorModel::UpdateResult ObjectInspectorModel::update(QDesignerFormWindowInterface *fw)
    {
        QWidget *mainContainer = fw ? fw->mainContainer() : static_cast<QWidget*>(0);
        if (!mainContainer) {
            clearItems();
            m_formWindow = 0;
            return NoForm;
        }
        m_formWindow = fw;
        // Build new model and compare to previous one. If the structure is
        // identical, just update, else rebuild
        ObjectModel newModel;

        static const QString separator = QCoreApplication::translate("ObjectInspectorModel", "separator");
        const ModelRecursionContext ctx(fw->core(),  separator);
        createModelRecursion(fw, 0, mainContainer, newModel, ctx);

        if (newModel == m_model) {
            updateItemContents(m_model, newModel);
            return Updated;
        }

        rebuild(newModel);
        m_model = newModel;
        return Rebuilt;
    }

    QObject *ObjectInspectorModel::objectAt(const QModelIndex &index) const
    {
        if (index.isValid())
            if (const QStandardItem *item = itemFromIndex(index))
                return objectOfItem(item);
        return 0;
    }

    // Missing Qt API: get a row
    ObjectInspectorModel::StandardItemList ObjectInspectorModel::rowAt(QModelIndex index) const
    {
        StandardItemList rc;
        while (true) {
            rc += itemFromIndex(index);
            const int nextColumn = index.column() + 1;
            if (nextColumn >=  NumColumns)
                break;
            index = index.sibling(index.row(), nextColumn);
        }
        return rc;
    }

    // Rebuild the tree in case the model has completely changed.
    void ObjectInspectorModel::rebuild(const ObjectModel &newModel)
    {
        clearItems();
        if (newModel.empty())
            return;

        const ObjectModel::const_iterator mcend = newModel.constEnd();
        ObjectModel::const_iterator it = newModel.constBegin();
        // Set up root element
        StandardItemList rootRow = createModelRow(it->object());
        it->setItems(rootRow, m_icons);
        appendRow(rootRow);
        m_objectIndexMultiMap.insert(it->object(), indexFromItem(rootRow.front()));
        for (++it; it != mcend; ++it) {
            // Add to parent item, found via map
            const QModelIndex parentIndex = m_objectIndexMultiMap.value(it->parent(), QModelIndex());
            Q_ASSERT(parentIndex.isValid());
            QStandardItem *parentItem = itemFromIndex(parentIndex);
            StandardItemList row = createModelRow(it->object());
            it->setItems(row, m_icons);
            parentItem->appendRow(row);
            m_objectIndexMultiMap.insert(it->object(), indexFromItem(row.front()));
        }
    }

    // Update item data in case the model has the same structure
    void ObjectInspectorModel::updateItemContents(ObjectModel &oldModel, const ObjectModel &newModel)
    {
        // Change text and icon. Keep a set of changed object
        // as for example actions might occur several times in the tree.
        typedef QSet<QObject *> QObjectSet;

        QObjectSet changedObjects;

        const int size = newModel.size();
        Q_ASSERT(oldModel.size() ==  size);
        for (int i = 0; i < size; i++) {
            const ObjectData &newEntry = newModel[i];
            ObjectData &entry =  oldModel[i];
            // Has some data changed?
            if (const unsigned changedMask = entry.compare(newEntry)) {
                entry = newEntry;
                QObject * o = entry.object();
                if (!changedObjects.contains(o)) {
                    changedObjects.insert(o);
                    const QModelIndexList indexes =  m_objectIndexMultiMap.values(o);
                    foreach (const QModelIndex &index, indexes)
                        entry.setItemsDisplayData(rowAt(index), m_icons, changedMask);
                }
            }
        }
    }

    QVariant ObjectInspectorModel::data(const QModelIndex &index, int role) const
    {
        const QVariant rc = QStandardItemModel::data(index, role);
        // Return <noname> if the string is empty for the display role
        // only (else, editing starts with <noname>).
        if (role == Qt::DisplayRole && rc.type() == QVariant::String) {
            const QString s = rc.toString();
            if (s.isEmpty()) {
                static const QString noName = QCoreApplication::translate("ObjectInspectorModel", "<noname>");
                return  QVariant(noName);
            }
        }
        return rc;
    }

    bool ObjectInspectorModel::setData(const QModelIndex &index, const QVariant &value, int role)
    {
        if (role != Qt::EditRole || !m_formWindow)
            return false;

        QObject *object = objectAt(index);
        if (!object)
            return false;
        // Is this a layout widget?
        const QString nameProperty = isQLayoutWidget(object) ? QLatin1String("layoutName") : QLatin1String("objectName");
        m_formWindow->commandHistory()->push(createTextPropertyCommand(nameProperty, value.toString(), object, m_formWindow));
        return true;
    }
}

QT_END_NAMESPACE
