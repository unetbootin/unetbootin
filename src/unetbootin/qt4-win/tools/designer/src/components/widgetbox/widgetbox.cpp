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
TRANSLATOR qdesigner_internal::WidgetBoxTreeView
*/

#include "widgetbox.h"
#include "widgetbox_dnditem.h"

// shared
#include <pluginmanager_p.h>
#include <sheet_delegate_p.h>
#include <iconloader_p.h>
#include <ui4_p.h>
#include <qdesigner_utils_p.h>

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerFormWindowManagerInterface>
#include <QtDesigner/QDesignerCustomWidgetInterface>
#include <QtDesigner/QDesignerWidgetDataBaseInterface>

#include <QtGui/QApplication>
#include <QtGui/QTreeWidget>
#include <QtGui/QHeaderView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QMenu>
#include <QtGui/QLineEdit>
#include <QtGui/qevent.h>

#include <QtXml/QDomDocument>
#include <QtCore/QFile>
#include <QtCore/qdebug.h>
#include <QtCore/QSettings>

QT_BEGIN_NAMESPACE

namespace {
    enum TopLevelRole  { NORMAL_ITEM, SCRATCHPAD_ITEM=1,CUSTOM_ITEM=2 };
    typedef QList<QDomElement> ElementList;
}

static const char *widgetBoxRootElementC = "widgetbox";
static const char *widgetElementC = "widget";
static const char *uiElementC = "ui";
static const char *categoryElementC = "category";
static const char *nameAttributeC = "name";
static const char *typeAttributeC = "type";
static const char *iconAttributeC = "icon";
static const char *defaultTypeValueC = "default";
static const char *customValueC = "custom";
static const char *iconPrefixC = "__qt_icon__";
static const char *scratchPadValueC = "scratchpad";
static const char *qtLogoC = "qtlogo.png";
static const char *invisibleNameC = "[invisible]";

static const char *uiOpeningTagC = "<ui>";
static const char *uiClosingTagC = "</ui>";

/*******************************************************************************
** Tools
*/
static QDomElement childElement(const QDomNode &node, const QString &tag,
                                const QString &attr_name,
                                const QString &attr_value)
{
    if (node.isElement()) {
       const  QDomElement elt = node.toElement();
        if (elt.tagName() == tag) {
            if (attr_name.isEmpty())
                return elt;
            if (elt.hasAttribute(attr_name)) {
                if (attr_value.isEmpty())
                    return elt;
                if (elt.attribute(attr_name) == attr_value)
                    return elt;
            }
        }
    }

    QDomNode child = node.firstChild();
    for (; !child.isNull(); child = child.nextSibling()) {
        const  QDomElement elt = childElement(child, tag, attr_name, attr_value);
        if (!elt.isNull())
            return elt;
    }

    return QDomElement();
}

static void _childElementList(const QDomNode &node, const QString &tag,
                                    const QString &attr_name,
                                    const QString &attr_value,
                                    ElementList *result)
{
    if (node.isElement()) {
        const  QDomElement elt = node.toElement();
        if (elt.tagName() == tag) {
            if (attr_name.isEmpty()) {
                result->append(elt);
            } else if (elt.hasAttribute(attr_name)) {
                if (attr_value.isEmpty())
                    result->append(elt);
                else if (elt.attribute(attr_name) == attr_value)
                    result->append(elt);
            }
        }
    }

    QDomNode child = node.firstChild();
    for (; !child.isNull(); child = child.nextSibling())
        _childElementList(child, tag, attr_name, attr_value, result);
}

static QString domToString(const QDomElement &elt)
{
    QString result;
    QTextStream stream(&result, QIODevice::WriteOnly);
    elt.save(stream, 2);
    stream.flush();
    return result;
}

static QDomDocument stringToDom(const QString &xml)
{
    QDomDocument result;
    result.setContent(xml);
    return result;
}

static void setTopLevelRole(TopLevelRole tlr, QTreeWidgetItem *item)
{
    item->setData(0, Qt::UserRole, QVariant(tlr));
}

static TopLevelRole topLevelRole(const  QTreeWidgetItem *item)
{
    return static_cast<TopLevelRole>(item->data(0, Qt::UserRole).toInt());
}

/*******************************************************************************
** WidgetBoxItemDelegate
*/

namespace qdesigner_internal {

class WidgetBoxItemDelegate : public SheetDelegate
{
public:
    WidgetBoxItemDelegate(QTreeWidget *tree, QWidget *parent = 0)
        : SheetDelegate(tree, parent) {}
    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
};

/*******************************************************************************
** WidgetBoxTreeView
*/

class WidgetBoxTreeView : public QTreeWidget
{
    Q_OBJECT

public:
    typedef QDesignerWidgetBoxInterface::Widget Widget;
    typedef QDesignerWidgetBoxInterface::Category Category;
    typedef QDesignerWidgetBoxInterface::CategoryList CategoryList;

    WidgetBoxTreeView(QDesignerFormEditorInterface *core, QWidget *parent = 0);
    ~WidgetBoxTreeView();

    int categoryCount() const;
    Category category(int cat_idx) const;
    void addCategory(const Category &cat);
    void removeCategory(int cat_idx);

    int widgetCount(int cat_idx) const;
    Widget widget(int cat_idx, int wgt_idx) const;
    void addWidget(int cat_idx, const Widget &wgt);
    void removeWidget(int cat_idx, int wgt_idx);

    void dropWidgets(const QList<QDesignerDnDItemInterface*> &item_list);

    void setFileName(const QString &file_name);
    QString fileName() const;
    bool load(WidgetBox::LoadMode loadMode);
    bool loadContents(const QString &contents, const QString &fileName);
    bool save();

signals:
    void pressed(const QString name, const QString dom_xml, bool custom, const QPoint &global_mouse_pos);

protected:
    void contextMenuEvent(QContextMenuEvent *e);

private slots:
    void handleMousePress(QTreeWidgetItem *item);
    void removeCurrentItem();
    void editCurrentItem();
    void updateItemData(QTreeWidgetItem *item);
    void deleteScratchpad();

private:
    QDesignerFormEditorInterface *m_core;
    QString m_file_name;
    mutable QHash<QString, QIcon> m_pluginIcons;
    QStringList m_widgetNames;

    CategoryList domToCategoryList(const QDomDocument &doc) const;
    Category domToCategory(const QDomElement &cat_elt) const;
    CategoryList loadCustomCategoryList() const;
    QDomDocument categoryListToDom(const CategoryList &cat_list) const;

    QTreeWidgetItem *widgetToItem(const Widget &wgt, QTreeWidgetItem *parent,
                                    bool editable = false);
    static Widget itemToWidget(const QTreeWidgetItem *item);

    int indexOfCategory(const QString &name) const;
    int indexOfScratchpad() const;
    int ensureScratchpad();
    void addCustomCategories(bool replace);

    void saveExpandedState() const;
    void restoreExpandedState();

    static QString widgetDomXml(const Widget &widget);
};


QWidget *WidgetBoxItemDelegate::createEditor(QWidget *parent,
                                                const QStyleOptionViewItem &option,
                                                const QModelIndex &index) const
{
    QWidget *result = SheetDelegate::createEditor(parent, option, index);
    QLineEdit *line_edit = qobject_cast<QLineEdit*>(result);
    if (line_edit == 0)
        return result;
    line_edit->setValidator(new QRegExpValidator(QRegExp(QLatin1String("[_a-zA-Z][_a-zA-Z0-9]*")), line_edit));
    return result;
}


WidgetBoxTreeView::WidgetBoxTreeView(QDesignerFormEditorInterface *core, QWidget *parent)
    : QTreeWidget(parent),m_core(core)
{
    setFocusPolicy(Qt::NoFocus);
    setIconSize(QSize(22, 22));

    setItemDelegate(new WidgetBoxItemDelegate(this, this));
    setRootIsDecorated(false);
    setColumnCount(1);
    header()->hide();
    header()->setResizeMode(QHeaderView::Stretch);
    setTextElideMode (Qt::ElideMiddle);

    connect(this, SIGNAL(itemPressed(QTreeWidgetItem*,int)),
            this, SLOT(handleMousePress(QTreeWidgetItem*)));
    connect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
            this, SLOT(updateItemData(QTreeWidgetItem*)));

    setEditTriggers(QAbstractItemView::AnyKeyPressed);
}

void WidgetBoxTreeView::saveExpandedState() const
{
    QStringList closedCategories;
    if (const int numCategories = categoryCount()) {
        for (int i = 0; i < numCategories; ++i) {
            const QTreeWidgetItem *cat_item = topLevelItem(i);
            if (!isItemExpanded(cat_item))
                closedCategories.append(cat_item->text(0));
        }
    }
    QSettings settings;
    settings.beginGroup(QLatin1String(widgetBoxRootElementC));
    settings.setValue(QLatin1String("Closed categories"), closedCategories);
    settings.endGroup();
}

void  WidgetBoxTreeView::restoreExpandedState()
{
    typedef QSet<QString> StringSet;
    QSettings settings;
    const StringSet closedCategories = settings.value(QLatin1String("WidgetBox/Closed categories"), QStringList()).toStringList().toSet();
    expandAll();
    if (closedCategories.empty())
        return;

    if (const int numCategories = categoryCount()) {
        for (int i = 0; i < numCategories; ++i) {
            QTreeWidgetItem *item = topLevelItem(i);
            if (closedCategories.contains(item->text(0)))
                item->setExpanded(false);
            }
    }
}

WidgetBoxTreeView::~WidgetBoxTreeView()
{
    saveExpandedState();
}

QString WidgetBoxTreeView::widgetDomXml(const Widget &widget)
{
    QString domXml = widget.domXml();

    if (domXml.isEmpty()) {
        const QString defaultVarName = qtify(widget.name());
        const QString typeStr = widget.type() == Widget::Default
                            ? QLatin1String(defaultTypeValueC)
                            : QLatin1String(customValueC);

        domXml = QLatin1String(uiOpeningTagC);
        domXml += QString::fromUtf8("<widget class=\"%1\" name=\"%2\" type=\"%3\"/>")
            .arg(widget.name())
            .arg(defaultVarName)
            .arg(typeStr);
        domXml += QLatin1String(uiClosingTagC);
    }
    return domXml;
}

void WidgetBoxTreeView::setFileName(const QString &file_name)
{
    m_file_name = file_name;
}

QString WidgetBoxTreeView::fileName() const
{
    return m_file_name;
}

bool WidgetBoxTreeView::save()
{
    if (fileName().isEmpty())
        return false;

    QFile file(fileName());
    if (!file.open(QIODevice::WriteOnly))
        return false;

    CategoryList cat_list;
    const int count = categoryCount();
    for (int i = 0; i < count; ++i)
        cat_list.append(category(i));

    const QDomDocument doc = categoryListToDom(cat_list);
    QTextStream stream(&file);
    doc.save(stream, 4);

    return true;
}

void WidgetBoxTreeView::handleMousePress(QTreeWidgetItem *item)
{
    if (item == 0)
        return;

    if (item->parent() == 0) {
        setItemExpanded(item, !isItemExpanded(item));
        return;
    }

    QDesignerWidgetBoxInterface::Widget wgt = itemToWidget(item);
    if (wgt.isNull())
        return;
    emit pressed(wgt.name(), widgetDomXml(wgt), wgt.type() == QDesignerWidgetBoxInterface::Widget::Custom, QCursor::pos());
}

int WidgetBoxTreeView::ensureScratchpad()
{
    const int existingIndex = indexOfScratchpad();
    if (existingIndex != -1)
         return existingIndex;

    QTreeWidgetItem *scratch_item = new QTreeWidgetItem(this);
    scratch_item->setText(0, tr("Scratchpad"));
    setTopLevelRole(SCRATCHPAD_ITEM, scratch_item);
    return categoryCount() - 1;
}

int WidgetBoxTreeView::indexOfScratchpad() const
{
    if (const int numTopLevels =  topLevelItemCount()) {
        for (int i = numTopLevels - 1; i >= 0; --i) {
            if (topLevelRole(topLevelItem(i)) == SCRATCHPAD_ITEM)
                return i;
        }
    }
    return -1;
}

int WidgetBoxTreeView::indexOfCategory(const QString &name) const
{
    const int topLevelCount = topLevelItemCount();
    for (int i = 0; i < topLevelCount; ++i) {
        if (topLevelItem(i)->text(0) == name)
            return i;
    }
    return -1;
}

bool WidgetBoxTreeView::load(WidgetBox::LoadMode loadMode)
{
    switch (loadMode) {
    case WidgetBox::LoadReplace:
        clear();
        break;
    case WidgetBox::LoadCustomWidgetsOnly:
        addCustomCategories(true);
        return true;
    default:
        break;
    }

    const QString name = fileName();

    QFile f(name);
    if (!f.open(QIODevice::ReadOnly)) {
        const QString msg = QObject::tr("The widgetbox could not load the file %1.").arg(name);
        return false;
    }
    const QString contents = QString::fromUtf8(f.readAll());
    return loadContents(contents, name);
}

bool WidgetBoxTreeView::loadContents(const QString &contents, const QString &fileName)
{
    QString error_msg;
    int line, col;
    QDomDocument doc;
    if (!doc.setContent(contents, &error_msg, &line, &col)) {
        if (!fileName.isEmpty()) {
            const QString msg = QObject::tr("The widgetbox could not parse the file %1. An error occurred at line %2: %3").
                arg(fileName).arg(line).arg(error_msg);
            qdesigner_internal::designerWarning(msg);
        }
        return false;
    }

    const CategoryList cat_list = domToCategoryList(doc);
    if (cat_list.isEmpty())
        return false;

    foreach(Category cat, cat_list)
        addCategory(cat);

    addCustomCategories(false);
    // Restore which items are expanded
    restoreExpandedState();
    return true;
}

void WidgetBoxTreeView::addCustomCategories(bool replace)
{
    if (replace) {
        // clear out all existing custom widgets
        if (const int numTopLevels =  topLevelItemCount()) {
            for (int t = 0; t < numTopLevels ; ++t) {
                QTreeWidgetItem *topLevel = topLevelItem(t);
                if (const int childCount = topLevel->childCount()) {
                    for (int c = childCount - 1; c >= 0; --c) {
                        QTreeWidgetItem *item = topLevel->child(c);
                        const QDesignerWidgetBoxInterface::Widget wgt = itemToWidget(item);
                        if (!wgt.isNull() && wgt.type() == QDesignerWidgetBoxInterface::Widget::Custom) {
                            delete item;
                            const int listIndex =  m_widgetNames.indexOf(wgt.name());
                            if (listIndex != -1)
                                m_widgetNames.removeAt(listIndex);
                        }
                    }
                }
            }
        }
    }
    // re-add
    foreach (Category cat, loadCustomCategoryList())
        addCategory(cat);
}

QDomDocument WidgetBoxTreeView::categoryListToDom(const CategoryList &cat_list) const
{
    QDomDocument doc;
    QDomElement root = doc.createElement(QLatin1String(widgetBoxRootElementC));
    doc.appendChild(root);

    const QString name = QLatin1String(nameAttributeC);
    const QString type = QLatin1String(typeAttributeC);
    const QString icon = QLatin1String(iconAttributeC);
    const QString defaultType = QLatin1String(defaultTypeValueC);
    const QString category = QLatin1String(categoryElementC);
    const QString iconPrefix = QLatin1String(iconPrefixC);
    const QString widgetTag = QLatin1String(widgetElementC);

    foreach (Category cat, cat_list) {
        QDomElement cat_elt = doc.createElement(category);
        root.appendChild(cat_elt);
        cat_elt.setAttribute(name, cat.name());
        if (cat.type() == Category::Scratchpad)
            cat_elt.setAttribute(type, QLatin1String(scratchPadValueC));
        const int widgetCount = cat.widgetCount();
        for (int i = 0; i < widgetCount; ++i) {
           const  Widget wgt = cat.widget(i);
            if (wgt.type() == Widget::Custom)
                continue;

            const DomUI *domUI = QDesignerWidgetBox::xmlToUi(wgt.name(), widgetDomXml(wgt), false);
            QDomElement uiElement = domUI->write(doc);
            QDomElement widgetElement = uiElement.firstChildElement(widgetTag);
            if (!widgetElement.isNull()) {
                widgetElement.setAttribute(name, wgt.name());
                const QString iconName = wgt.iconName();
                if (!iconName.startsWith(iconPrefix))
                    widgetElement.setAttribute(icon, wgt.iconName());
                widgetElement.setAttribute(type, defaultType);
            }
            cat_elt.appendChild(uiElement);
            delete domUI;
        }
    }

    return doc;
}

WidgetBoxTreeView::CategoryList
    WidgetBoxTreeView::domToCategoryList(const QDomDocument &doc) const
{
    CategoryList result;

    const QDomElement root = doc.firstChildElement();
    if (root.nodeName() != QLatin1String(widgetBoxRootElementC)) {
        qdesigner_internal::designerWarning(QObject::tr("The file %1 does not appear to be a widgetbox file.").arg(m_file_name));
        return result;
    }

    QDomElement cat_elt = root.firstChildElement();
    for (; !cat_elt.isNull(); cat_elt = cat_elt.nextSiblingElement()) {
        if (cat_elt.nodeName() != QLatin1String(categoryElementC)) {
            qdesigner_internal::designerWarning(QObject::tr("An error occurred while parsing the file %1: %2 is not a valid child of the root element.").
                                                arg(m_file_name).arg( cat_elt.nodeName()));
            return result;
        }

        const Category cat = domToCategory(cat_elt);
        if (!cat.isNull())
            result.append(cat);
    }

    return result;
}

WidgetBoxTreeView::Category WidgetBoxTreeView::domToCategory(const QDomElement &cat_elt) const
{
    const QString nameAttribute = QLatin1String(nameAttributeC);
    const QString type = QLatin1String(typeAttributeC);
    const QString icon = QLatin1String(iconAttributeC);
    const QString name = cat_elt.attribute(nameAttribute);
    const QString custom = QLatin1String(customValueC);
    const QString uiTag = QLatin1String(uiElementC);
    const QString widgetTag = QLatin1String(widgetElementC);

    if (name == QLatin1String(invisibleNameC))
        return Category();

    Category result(name);

    if (cat_elt.attribute(type) == QLatin1String(scratchPadValueC))
        result.setType(Category::Scratchpad);

    QDomElement childElement = cat_elt.firstChildElement();
    for (; !childElement.isNull(); childElement = childElement.nextSiblingElement()) {
        // Support legacy <widget> tags as well as <ui> tags
        QDomElement widgetElement;
        QString xml;
        if (childElement.nodeName() == uiTag) {
            widgetElement = childElement.firstChildElement(widgetTag);
            xml = domToString(childElement);
        } else { // pre 4.4
            widgetElement = childElement;
            xml = domToString(widgetElement).trimmed();
            xml.insert(0, QLatin1String(uiOpeningTagC));
            xml.append(QLatin1String(uiClosingTagC));
        }
        if (!widgetElement.isNull()) {
            const QString type_attr = widgetElement.attribute(type);
            const Widget::Type type = type_attr == custom ? Widget::Custom : Widget::Default;
            const Widget w(widgetElement.attribute(nameAttribute), xml, widgetElement.attribute(icon), type);
            result.addWidget(w);
        }
    }

    return result;
}

static int findCategory(const QString &name, const WidgetBoxTreeView::CategoryList &list)
{
    int idx = 0;
    foreach (const WidgetBoxTreeView::Category &cat, list) {
        if (cat.name() == name)
            return idx;
        ++idx;
    }
    return -1;
}

WidgetBoxTreeView::CategoryList WidgetBoxTreeView::loadCustomCategoryList() const
{
    CategoryList result;

    QDesignerPluginManager *pm = m_core->pluginManager();

    const QList<QDesignerCustomWidgetInterface*> customWidgets = pm->registeredCustomWidgets();

    static const QString customCatName = tr("Custom Widgets");
    const QString invisible = QLatin1String(invisibleNameC);
    const QString iconPrefix = QLatin1String(iconPrefixC);

    foreach (const QDesignerCustomWidgetInterface *c, customWidgets) {
        const QString dom_xml = c->domXml();
        if (dom_xml.isEmpty())
            continue;

        QString cat_name = c->group();
        if (cat_name.isEmpty())
            cat_name = customCatName;
        else if (cat_name == invisible)
            continue;

        int idx = findCategory(cat_name, result);
        if (idx == -1) {
            result.append(Category(cat_name));
            idx = result.size() - 1;
        }
        Category &cat = result[idx];

        const QIcon icon = c->icon();

        QString icon_name;
        if (icon.isNull())
            icon_name = QLatin1String(qtLogoC);
        else {
            icon_name = iconPrefix;
            icon_name += c->name();
            m_pluginIcons.insert(icon_name, icon);
        }

        cat.addWidget(Widget(c->name(), dom_xml, icon_name, Widget::Custom));
    }

    return result;
}

QTreeWidgetItem *WidgetBoxTreeView::widgetToItem(const Widget &wgt,
                                                    QTreeWidgetItem *parent,
                                                    bool editable)
{
    if (!editable && m_widgetNames.contains(wgt.name()))
        return 0;

    QTreeWidgetItem *item = new QTreeWidgetItem(parent);
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);

    QString icon_name = wgt.iconName();
    if (icon_name.isEmpty())
        icon_name = QLatin1String(qtLogoC);

    const bool block = blockSignals(true);
    item->setText(0, wgt.name());

    if (!editable)
        m_widgetNames.append(wgt.name());

    QIcon icon;
    if (icon_name.startsWith(QLatin1String(iconPrefixC)))
      icon = m_pluginIcons.value(icon_name);
    if (icon.isNull())
      icon = createIconSet(icon_name);
    item->setIcon(0, icon);
    item->setData(0, Qt::UserRole, qVariantFromValue(wgt));

    const QDesignerWidgetDataBaseInterface *db = m_core->widgetDataBase();
    const int dbIndex = db->indexOfClassName(wgt.name());
    if (dbIndex != -1) {
        const QDesignerWidgetDataBaseItemInterface *dbItem = db->item(dbIndex);
        const QString toolTip = dbItem->toolTip();
        if (!toolTip.isEmpty())
            item->setToolTip(0, toolTip);
        const QString whatsThis = dbItem->whatsThis();
        if (!whatsThis.isEmpty())
            item->setWhatsThis(0, whatsThis);
    }

    blockSignals(block);

    if (editable)
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);

    return item;
}

WidgetBoxTreeView::Widget WidgetBoxTreeView::itemToWidget(const QTreeWidgetItem *item)
{
    return qvariant_cast<Widget>(item->data(0, Qt::UserRole));
}

int WidgetBoxTreeView::categoryCount() const
{
    return topLevelItemCount();
}

WidgetBoxTreeView::Category WidgetBoxTreeView::category(int cat_idx) const
{
    Category result;

    if (cat_idx >= topLevelItemCount())
        return result;

    QTreeWidgetItem *cat_item = topLevelItem(cat_idx);
    result.setName(cat_item->text(0));

    const int childCount = cat_item->childCount();
    for (int i = 0; i < childCount; ++i) {
        QTreeWidgetItem *child = cat_item->child(i);
        result.addWidget(itemToWidget(child));
    }

    switch (topLevelRole(cat_item)) {
    case SCRATCHPAD_ITEM:
        result.setType(Category::Scratchpad);
        break;
    default:
        result.setType(Category::Default);
        break;
    }
    return result;
}

void WidgetBoxTreeView::addCategory(const Category &cat)
{
    if (cat.widgetCount() == 0)
        return;

    const bool isScratchPad = cat.type() == Category::Scratchpad;
    QTreeWidgetItem *cat_item = 0;

    if (isScratchPad) {
        cat_item = topLevelItem(ensureScratchpad());
    } else {
        const int existingIndex = indexOfCategory(cat.name());
        if (existingIndex == -1) {
            cat_item = new QTreeWidgetItem();
            cat_item->setText(0, cat.name());
            setTopLevelRole(NORMAL_ITEM, cat_item);
            // insert before scratchpad
            const int scratchPadIndex = indexOfScratchpad();
            if (scratchPadIndex == -1) {
                addTopLevelItem(cat_item);
            } else {
                insertTopLevelItem(scratchPadIndex, cat_item);
            }
            setItemExpanded(cat_item, true);
        } else {
            cat_item = topLevelItem(existingIndex);
        }
    }
    const int widgetCount = cat.widgetCount();
    for (int i = 0; i < widgetCount; ++i)
        widgetToItem(cat.widget(i), cat_item, isScratchPad);
}

void WidgetBoxTreeView::removeCategory(int cat_idx)
{
    if (cat_idx >= topLevelItemCount())
        return;
    delete takeTopLevelItem(cat_idx);
}

int WidgetBoxTreeView::widgetCount(int cat_idx) const
{
    if (cat_idx >= topLevelItemCount())
        return 0;

    return topLevelItem(cat_idx)->childCount();
}

WidgetBoxTreeView::Widget WidgetBoxTreeView::widget(int cat_idx, int wgt_idx) const
{
    if (cat_idx >= topLevelItemCount())
        return Widget();

    QTreeWidgetItem *cat_item = topLevelItem(cat_idx);

    if (wgt_idx >= cat_item->childCount())
        return Widget();

    return itemToWidget(cat_item->child(wgt_idx));
}

void WidgetBoxTreeView::addWidget(int cat_idx, const Widget &wgt)
{
    if (cat_idx >= topLevelItemCount())
        return;

    QTreeWidgetItem *cat_item = topLevelItem(cat_idx);

    const bool scratch = topLevelRole(cat_item) == SCRATCHPAD_ITEM;
    widgetToItem(wgt, cat_item, scratch);
}

void WidgetBoxTreeView::removeWidget(int cat_idx, int wgt_idx)
{
    if (cat_idx >= topLevelItemCount())
        return;

    QTreeWidgetItem *cat_item = topLevelItem(cat_idx);

    if (wgt_idx >= cat_item->childCount())
        return;

    delete cat_item->takeChild(wgt_idx);
}

void WidgetBoxTreeView::removeCurrentItem()
{
    QTreeWidgetItem *item = currentItem();
    if (item == 0)
        return;

    QTreeWidgetItem *parent = item->parent();
    if (parent == 0) {
        takeTopLevelItem(indexOfTopLevelItem(item));
    } else {
        parent->takeChild(parent->indexOfChild(item));
        setItemExpanded(parent, true);
        if (topLevelRole(parent) == SCRATCHPAD_ITEM
                && parent->childCount() == 0) {
            QMetaObject::invokeMethod(this, "deleteScratchpad",
                                        Qt::QueuedConnection);
        }
    }
    delete item;

    save();
}

void WidgetBoxTreeView::deleteScratchpad()
{
    const int idx = indexOfScratchpad();
    if (idx == -1)
        return;
    delete takeTopLevelItem(idx);
}

void WidgetBoxTreeView::updateItemData(QTreeWidgetItem *item)
{
    if (item->parent() == 0)
        return;

    Widget widget = qvariant_cast<Widget>(item->data(0, Qt::UserRole));

    if (item->text(0).isEmpty()) {
        QString widgetName = widget.name();
        if (!widgetName.isEmpty())
            item->setText(0, widgetName);
        return;
    }

    widget.setName(item->text(0));
    const QDomDocument doc = stringToDom(widgetDomXml(widget));
    QDomElement widget_elt = doc.firstChildElement(QLatin1String(widgetElementC));
    if (!widget_elt.isNull()) {
        widget_elt.setAttribute(QLatin1String(nameAttributeC), item->text(0));
        widget.setDomXml(domToString(widget_elt));
    }

    const bool block = blockSignals(true);
    item->setData(0, Qt::UserRole, qVariantFromValue(widget));
    blockSignals(block);

    save();
}

void WidgetBoxTreeView::editCurrentItem()
{
    QModelIndex index = currentIndex();
    if (!index.isValid())
        return;

    edit(index);
}

void WidgetBoxTreeView::contextMenuEvent(QContextMenuEvent *e)
{
    QTreeWidgetItem *item = itemAt(e->pos());

    const bool scratchpad_menu = item != 0
                            && item->parent() != 0
                            && topLevelRole(item->parent()) ==  SCRATCHPAD_ITEM;

    QMenu menu;
    menu.addAction(tr("Expand all"), this, SLOT(expandAll()));
    menu.addAction(tr("Collapse all"), this, SLOT(collapseAll()));
    menu.addSeparator();
    if (scratchpad_menu) {
        setCurrentItem(item);
        menu.addAction(tr("Remove"), this, SLOT(removeCurrentItem()));
        menu.addAction(tr("Edit name"), this, SLOT(editCurrentItem()));
    }
    e->accept();
    menu.exec(mapToGlobal(e->pos()));
}

void WidgetBoxTreeView::dropWidgets(const QList<QDesignerDnDItemInterface*> &item_list)
{
    QTreeWidgetItem *last_item = 0;

    foreach (QDesignerDnDItemInterface *item, item_list) {
        QWidget *w = item->widget();
        if (w == 0)
            continue;

        DomUI *dom_ui = item->domUi();
        if (dom_ui == 0)
            continue;

        const int scratch_idx = ensureScratchpad();
        QTreeWidgetItem *scratch_item = topLevelItem(scratch_idx);

        QDomDocument dom;
        QDomElement uiElement = dom_ui->write(dom);
        // Remove the fake toplevel in-between
        const QString widgetElement = QLatin1String(widgetElementC);
        QDomElement fakeTopLevel = uiElement.firstChildElement(widgetElement);
        QDomElement firstWidget = fakeTopLevel.firstChildElement(widgetElement);
        uiElement.replaceChild(firstWidget, fakeTopLevel);
        const QString xml = domToString(uiElement);
        last_item = widgetToItem(Widget(w->objectName(), xml), scratch_item, true);
        setItemExpanded(scratch_item, true);
    }

    if (last_item != 0) {
        save();
        QApplication::setActiveWindow(this);
        setCurrentItem(last_item);
    }
}

/*******************************************************************************
** WidgetBox
*/

WidgetBox::WidgetBox(QDesignerFormEditorInterface *core, QWidget *parent, Qt::WindowFlags flags)
    : QDesignerWidgetBox(parent, flags),
      m_core(core),
      m_view(new WidgetBoxTreeView(m_core))
{

    QVBoxLayout *l = new QVBoxLayout(this);
    l->setMargin(0);

    l->addWidget(m_view);

    connect(m_view, SIGNAL(pressed(QString,QString,bool,QPoint)),
            this, SLOT(handleMousePress(QString,QString,bool,QPoint)));

    setAcceptDrops (true);
}

WidgetBox::~WidgetBox()
{
}

QDesignerFormEditorInterface *WidgetBox::core() const
{
    return m_core;
}

void WidgetBox::handleMousePress(const QString &name, const QString &xml, bool custom, const QPoint &global_mouse_pos)
{
    if (QApplication::mouseButtons() != Qt::LeftButton)
        return;

    DomUI *ui = xmlToUi(name, xml, true);
    if (ui == 0)
        return;
    // Sanity check: Do the names match?
    if (custom) {
        DomWidget *dom_widget = ui->elementWidget()->elementWidget().front();
        if (dom_widget->hasAttributeClass()) {
            const QString domClassName = dom_widget->attributeClass();
            if (domClassName != name)
                qdesigner_internal::designerWarning(QObject::tr("The class attribute for the class %1 does not match the class name %2.").arg(domClassName).arg(name));
        } else {
                qdesigner_internal::designerWarning(QObject::tr("The class attribute for the class %1 is missing.").arg(name));
        }
    }

    QList<QDesignerDnDItemInterface*> item_list;
    item_list.append(new WidgetBoxDnDItem(core(), ui, global_mouse_pos));
    m_core->formWindowManager()->dragItems(item_list);
}

int WidgetBox::categoryCount() const
{
    return m_view->categoryCount();
}

QDesignerWidgetBoxInterface::Category WidgetBox::category(int cat_idx) const
{
    return m_view->category(cat_idx);
}

void WidgetBox::addCategory(const Category &cat)
{
    m_view->addCategory(cat);
}

void WidgetBox::removeCategory(int cat_idx)
{
    m_view->removeCategory(cat_idx);
}

int WidgetBox::widgetCount(int cat_idx) const
{
    return m_view->widgetCount(cat_idx);
}

QDesignerWidgetBoxInterface::Widget WidgetBox::widget(int cat_idx, int wgt_idx) const
{
    return m_view->widget(cat_idx, wgt_idx);
}

void WidgetBox::addWidget(int cat_idx, const Widget &wgt)
{
    m_view->addWidget(cat_idx, wgt);
}

void WidgetBox::removeWidget(int cat_idx, int wgt_idx)
{
    m_view->removeWidget(cat_idx, wgt_idx);
}

void WidgetBox::dropWidgets(const QList<QDesignerDnDItemInterface*> &item_list, const QPoint&)
{
    m_view->dropWidgets(item_list);
}

void WidgetBox::setFileName(const QString &file_name)
{
    m_view->setFileName(file_name);
}

QString WidgetBox::fileName() const
{
    return m_view->fileName();
}

bool WidgetBox::load()
{
    return m_view->load(loadMode());
}

bool WidgetBox::loadContents(const QString &contents)
{
    return m_view->loadContents(contents, QString());
}

bool WidgetBox::save()
{
    return m_view->save();
}

static const QDesignerMimeData *checkDragEvent(QDropEvent * event,
                                               bool acceptEventsFromWidgetBox)
{
    const QDesignerMimeData *mimeData = qobject_cast<const QDesignerMimeData *>(event->mimeData());
    if (!mimeData) {
        event->ignore();
        return 0;
    }
    // If desired, ignore a widget box drag and drop, where widget==0.
    if (!acceptEventsFromWidgetBox) {
        const bool fromWidgetBox = !mimeData->items().first()->widget();
        if (fromWidgetBox) {
            event->ignore();
            return 0;
        }
    }

    mimeData->acceptEvent(event);
    return mimeData;
}

void WidgetBox::dragEnterEvent (QDragEnterEvent * event)
{
    // We accept event originating from the widget box also here,
    // because otherwise Windows will not show the DnD pixmap.
    checkDragEvent(event, true);
}

void WidgetBox::dragMoveEvent(QDragMoveEvent * event)
{
    checkDragEvent(event, true);
}

void WidgetBox::dropEvent(QDropEvent * event)
{
    const QDesignerMimeData *mimeData = checkDragEvent(event, false);
    if (!mimeData)
        return;

    dropWidgets(mimeData->items(), event->pos());
    QDesignerMimeData::removeMovedWidgetsFromSourceForm(mimeData->items());
}
}  // namespace qdesigner_internal

QT_END_NAMESPACE

#include "widgetbox.moc"
