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
TRANSLATOR qdesigner_internal::ObjectInspector
*/

#include "objectinspectormodel_p.h"

#include <qlayout_widget_p.h>
#include <layout_p.h>
#include <qdesigner_propertycommand_p.h>
#include <qdesigner_utils_p.h>

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
#include <QtCore/QSet>
#include <QtCore/QDebug>

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

static inline QLayout *layoutOfQLayoutWidget(QObject *o)
{
    if (o->isWidgetType() && !qstrcmp(o->metaObject()->className(), "QLayoutWidget"))
        return static_cast<QWidget*>(o)->layout();
    return 0;
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

namespace qdesigner_internal {
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

    ObjectData::ObjectData(QObject *parent, QObject *object) :
       m_parent(parent),
       m_object(object)
    {
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
        if (!sameIcon(m_icon, rhs.m_icon))
            rc |= IconChanged;
        return rc;
    }

    void ObjectData::setItemsDisplayData(const StandardItemList &row, unsigned mask) const
    {
        if (mask & ObjectNameChanged)
            row[ObjectInspectorModel::ObjectNameColumn]->setText(m_objectName);
        if (mask & ClassNameChanged) {
            row[ObjectInspectorModel::ClassNameColumn]->setText(m_className);
            row[ObjectInspectorModel::ClassNameColumn]->setToolTip(m_className);
        }
        if (mask & IconChanged)
            row[ObjectInspectorModel::ObjectNameColumn]->setIcon(m_icon);
    }

    void ObjectData::setItems(const StandardItemList &row) const
    {
        const QVariant object = qVariantFromValue(m_object);
        row[ObjectInspectorModel::ObjectNameColumn]->setData(object, DataRole);
        row[ObjectInspectorModel::ClassNameColumn]->setData(object, DataRole);
        setItemsDisplayData(row, ClassNameChanged|ObjectNameChanged|IconChanged);
    }

    typedef QList<ObjectData> ObjectModel;

    // Recursive routine that creates the model by traversing the form window object tree.
    void createModelRecursion(const QDesignerFormWindowInterface *fwi, QObject *parent, QObject *object, ObjectModel &model)
    {
        const QString designerPrefix = QLatin1String("QDesigner");
        static const QString separator =  QObject::tr("separator");
        const QDesignerWidgetDataBaseInterface *db = fwi->core()->widgetDataBase();

        // 1) set entry
        const bool isWidget = object->isWidgetType();
        ObjectData entry(parent, object);

        entry.m_className = QLatin1String(object->metaObject()->className());
        // Is this a QLayoutWidget?
        const QLayout *layoutWidgetLayout = layoutOfQLayoutWidget(object);
        if (const QDesignerWidgetDataBaseItemInterface *widgetItem = db->item(db->indexOfObject(object, true))) {
            entry.m_icon = widgetItem->icon();
            if (layoutWidgetLayout) {
                entry.m_className = QLatin1String(layoutWidgetLayout->metaObject()->className());
            } else {
                entry.m_className = widgetItem->name();
            }
        }

        if (entry.m_className.startsWith(designerPrefix))
            entry.m_className.remove(1, designerPrefix.size() - 1);

        entry.m_objectName = layoutWidgetLayout ? layoutWidgetLayout->objectName() : object->objectName();

        if (const QAction *act = qobject_cast<const QAction*>(object)) { // separator is reserved
            if (act->isSeparator()) {
                entry.m_objectName = separator;
            }
            entry.m_icon = act->icon();
        }

        model.push_back(entry);
        // 2) recurse over children
        if (QDesignerContainerExtension *c = qt_extension<QDesignerContainerExtension*>(fwi->core()->extensionManager(), object)) {
            const int count = c->count();
            for (int i=0; i < count; ++i) {
                QObject *page = c->widget(i);
                Q_ASSERT(page != 0);
                createModelRecursion(fwi, object, page, model);
            }
        } else {
            QList<QObject*> children = object->children();
            if (!children.empty()) {
                qSort(children.begin(), children.end(), sortEntry);
                foreach (QObject *child, children)
                    if (child->isWidgetType()) {
                        QWidget *widget = qobject_cast<QWidget*>(child);
                        if (fwi->isManaged(widget))
                            createModelRecursion(fwi, object, widget, model);
                    }
            }
            if (isWidget) {
                QWidget *widget = qobject_cast<QWidget*>(object);
                const QList<QAction*> actions = widget->actions();
                foreach (QAction *action, actions) {
                    if (!fwi->core()->metaDataBase()->item(action))
                        continue;

                    QObject *obj = action;
                    if (action->menu())
                        obj = action->menu();

                    createModelRecursion(fwi, object, obj, model);
                }
            }
        }
    }

    // ------------ ObjectInspectorModel
    ObjectInspectorModel::ObjectInspectorModel(QObject *parent) :
       QStandardItemModel(0, NumColumns, parent)
    {
        QStringList headers;
        headers += QObject::tr("Object");
        headers += QObject::tr("Class");
        Q_ASSERT(headers.size() == NumColumns);
        setColumnCount(NumColumns);
        setHorizontalHeaderLabels(headers);
    }

    void ObjectInspectorModel::clearItems()
    {
        m_objectIndexMultiMap.clear();
        m_model.clear();
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
        createModelRecursion(fw, 0, mainContainer, newModel);

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
        StandardItemList rootRow = createModelRow(it->m_object);
        it->setItems(rootRow);
        appendRow(rootRow);
        m_objectIndexMultiMap.insert(it->m_object, indexFromItem(rootRow.front()));
        for (++it; it != mcend; ++it) {
            // Add to parent item, found via map
            const QModelIndex parentIndex = m_objectIndexMultiMap.value(it->m_parent, QModelIndex());
            Q_ASSERT(parentIndex.isValid());
            QStandardItem *parentItem = itemFromIndex(parentIndex);
            StandardItemList row = createModelRow(it->m_object);
            it->setItems(row);
            parentItem->appendRow(row);
            m_objectIndexMultiMap.insert(it->m_object, indexFromItem(row.front()));
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
                if (!changedObjects.contains(entry.m_object)) {
                    changedObjects.insert(entry.m_object);
                    const QModelIndexList indexes =  m_objectIndexMultiMap.values(entry.m_object);
                    foreach (const QModelIndex &index, indexes)
                        entry.setItemsDisplayData(rowAt(index), changedMask);
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
                static const QString noName = QObject::tr("<noname>");
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
        const QString nameProperty = layoutOfQLayoutWidget(object) ? QLatin1String("layoutName") : QLatin1String("objectName");
        m_formWindow->commandHistory()->push(createTextPropertyCommand(nameProperty, value.toString(), object, m_formWindow));
        return true;

QT_END_NAMESPACE
    }
}
