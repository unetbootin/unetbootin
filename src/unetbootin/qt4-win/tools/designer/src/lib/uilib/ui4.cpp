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
#include "ui4_p.h"
#include <QDomDocument>

QT_BEGIN_NAMESPACE
#ifdef QFORMINTERNAL_NAMESPACE
using namespace QFormInternal;
#endif

/*******************************************************************************
** Implementations
*/

void DomUI::clear(bool clear_all)
{
    delete m_widget;
    delete m_layoutDefault;
    delete m_layoutFunction;
    delete m_customWidgets;
    delete m_tabStops;
    delete m_images;
    delete m_includes;
    delete m_resources;
    delete m_connections;
    delete m_designerdata;
    delete m_slots;

    if (clear_all) {
    m_text = QString();
    m_has_attr_version = false;
    m_has_attr_language = false;
    m_has_attr_stdSetDef = false;
    m_attr_stdSetDef = 0;
    }

    m_children = 0;
    m_widget = 0;
    m_layoutDefault = 0;
    m_layoutFunction = 0;
    m_customWidgets = 0;
    m_tabStops = 0;
    m_images = 0;
    m_includes = 0;
    m_resources = 0;
    m_connections = 0;
    m_designerdata = 0;
    m_slots = 0;
}

DomUI::DomUI()
{
    m_children = 0;
    m_has_attr_version = false;
    m_has_attr_language = false;
    m_has_attr_stdSetDef = false;
    m_attr_stdSetDef = 0;
    m_widget = 0;
    m_layoutDefault = 0;
    m_layoutFunction = 0;
    m_customWidgets = 0;
    m_tabStops = 0;
    m_images = 0;
    m_includes = 0;
    m_resources = 0;
    m_connections = 0;
    m_designerdata = 0;
    m_slots = 0;
}

DomUI::~DomUI()
{
    delete m_widget;
    delete m_layoutDefault;
    delete m_layoutFunction;
    delete m_customWidgets;
    delete m_tabStops;
    delete m_images;
    delete m_includes;
    delete m_resources;
    delete m_connections;
    delete m_designerdata;
    delete m_slots;
}

void DomUI::read(const QDomElement &node)
{
    if (node.hasAttribute(QLatin1String("version")))
        setAttributeVersion(node.attribute(QLatin1String("version")));
    if (node.hasAttribute(QLatin1String("language")))
        setAttributeLanguage(node.attribute(QLatin1String("language")));
    if (node.hasAttribute(QLatin1String("stdsetdef")))
        setAttributeStdSetDef(node.attribute(QLatin1String("stdsetdef")).toInt());

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("author")) {
            setElementAuthor(e.text());
            continue;
        }
        if (tag == QLatin1String("comment")) {
            setElementComment(e.text());
            continue;
        }
        if (tag == QLatin1String("exportmacro")) {
            setElementExportMacro(e.text());
            continue;
        }
        if (tag == QLatin1String("class")) {
            setElementClass(e.text());
            continue;
        }
        if (tag == QLatin1String("widget")) {
            DomWidget *v = new DomWidget();
            v->read(e);
            setElementWidget(v);
            continue;
        }
        if (tag == QLatin1String("layoutdefault")) {
            DomLayoutDefault *v = new DomLayoutDefault();
            v->read(e);
            setElementLayoutDefault(v);
            continue;
        }
        if (tag == QLatin1String("layoutfunction")) {
            DomLayoutFunction *v = new DomLayoutFunction();
            v->read(e);
            setElementLayoutFunction(v);
            continue;
        }
        if (tag == QLatin1String("pixmapfunction")) {
            setElementPixmapFunction(e.text());
            continue;
        }
        if (tag == QLatin1String("customwidgets")) {
            DomCustomWidgets *v = new DomCustomWidgets();
            v->read(e);
            setElementCustomWidgets(v);
            continue;
        }
        if (tag == QLatin1String("tabstops")) {
            DomTabStops *v = new DomTabStops();
            v->read(e);
            setElementTabStops(v);
            continue;
        }
        if (tag == QLatin1String("images")) {
            DomImages *v = new DomImages();
            v->read(e);
            setElementImages(v);
            continue;
        }
        if (tag == QLatin1String("includes")) {
            DomIncludes *v = new DomIncludes();
            v->read(e);
            setElementIncludes(v);
            continue;
        }
        if (tag == QLatin1String("resources")) {
            DomResources *v = new DomResources();
            v->read(e);
            setElementResources(v);
            continue;
        }
        if (tag == QLatin1String("connections")) {
            DomConnections *v = new DomConnections();
            v->read(e);
            setElementConnections(v);
            continue;
        }
        if (tag == QLatin1String("designerdata")) {
            DomDesignerData *v = new DomDesignerData();
            v->read(e);
            setElementDesignerdata(v);
            continue;
        }
        if (tag == QLatin1String("slots")) {
            DomSlots *v = new DomSlots();
            v->read(e);
            setElementSlots(v);
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomUI::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("ui") : tagName.toLower());

    QDomElement child;

    if (hasAttributeVersion())
        e.setAttribute(QLatin1String("version"), attributeVersion());

    if (hasAttributeLanguage())
        e.setAttribute(QLatin1String("language"), attributeLanguage());

    if (hasAttributeStdSetDef())
        e.setAttribute(QLatin1String("stdsetdef"), attributeStdSetDef());

    if (m_children & Author) {
        child = doc.createElement(QLatin1String("author"));
        child.appendChild(doc.createTextNode(m_author));
        e.appendChild(child);
    }

    if (m_children & Comment) {
        child = doc.createElement(QLatin1String("comment"));
        child.appendChild(doc.createTextNode(m_comment));
        e.appendChild(child);
    }

    if (m_children & ExportMacro) {
        child = doc.createElement(QLatin1String("exportmacro"));
        child.appendChild(doc.createTextNode(m_exportMacro));
        e.appendChild(child);
    }

    if (m_children & Class) {
        child = doc.createElement(QLatin1String("class"));
        child.appendChild(doc.createTextNode(m_class));
        e.appendChild(child);
    }

    if (m_children & Widget) {
        e.appendChild(m_widget->write(doc, QLatin1String("widget")));
    }

    if (m_children & LayoutDefault) {
        e.appendChild(m_layoutDefault->write(doc, QLatin1String("layoutdefault")));
    }

    if (m_children & LayoutFunction) {
        e.appendChild(m_layoutFunction->write(doc, QLatin1String("layoutfunction")));
    }

    if (m_children & PixmapFunction) {
        child = doc.createElement(QLatin1String("pixmapfunction"));
        child.appendChild(doc.createTextNode(m_pixmapFunction));
        e.appendChild(child);
    }

    if (m_children & CustomWidgets) {
        e.appendChild(m_customWidgets->write(doc, QLatin1String("customwidgets")));
    }

    if (m_children & TabStops) {
        e.appendChild(m_tabStops->write(doc, QLatin1String("tabstops")));
    }

    if (m_children & Images) {
        e.appendChild(m_images->write(doc, QLatin1String("images")));
    }

    if (m_children & Includes) {
        e.appendChild(m_includes->write(doc, QLatin1String("includes")));
    }

    if (m_children & Resources) {
        e.appendChild(m_resources->write(doc, QLatin1String("resources")));
    }

    if (m_children & Connections) {
        e.appendChild(m_connections->write(doc, QLatin1String("connections")));
    }

    if (m_children & Designerdata) {
        e.appendChild(m_designerdata->write(doc, QLatin1String("designerdata")));
    }

    if (m_children & Slots) {
        e.appendChild(m_slots->write(doc, QLatin1String("slots")));
    }

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomUI::setElementAuthor(const QString& a)
{
    m_children |= Author;
    m_author = a;
}

void DomUI::setElementComment(const QString& a)
{
    m_children |= Comment;
    m_comment = a;
}

void DomUI::setElementExportMacro(const QString& a)
{
    m_children |= ExportMacro;
    m_exportMacro = a;
}

void DomUI::setElementClass(const QString& a)
{
    m_children |= Class;
    m_class = a;
}

void DomUI::setElementWidget(DomWidget* a)
{
    delete m_widget;
    m_children |= Widget;
    m_widget = a;
}

void DomUI::setElementLayoutDefault(DomLayoutDefault* a)
{
    delete m_layoutDefault;
    m_children |= LayoutDefault;
    m_layoutDefault = a;
}

void DomUI::setElementLayoutFunction(DomLayoutFunction* a)
{
    delete m_layoutFunction;
    m_children |= LayoutFunction;
    m_layoutFunction = a;
}

void DomUI::setElementPixmapFunction(const QString& a)
{
    m_children |= PixmapFunction;
    m_pixmapFunction = a;
}

void DomUI::setElementCustomWidgets(DomCustomWidgets* a)
{
    delete m_customWidgets;
    m_children |= CustomWidgets;
    m_customWidgets = a;
}

void DomUI::setElementTabStops(DomTabStops* a)
{
    delete m_tabStops;
    m_children |= TabStops;
    m_tabStops = a;
}

void DomUI::setElementImages(DomImages* a)
{
    delete m_images;
    m_children |= Images;
    m_images = a;
}

void DomUI::setElementIncludes(DomIncludes* a)
{
    delete m_includes;
    m_children |= Includes;
    m_includes = a;
}

void DomUI::setElementResources(DomResources* a)
{
    delete m_resources;
    m_children |= Resources;
    m_resources = a;
}

void DomUI::setElementConnections(DomConnections* a)
{
    delete m_connections;
    m_children |= Connections;
    m_connections = a;
}

void DomUI::setElementDesignerdata(DomDesignerData* a)
{
    delete m_designerdata;
    m_children |= Designerdata;
    m_designerdata = a;
}

void DomUI::setElementSlots(DomSlots* a)
{
    delete m_slots;
    m_children |= Slots;
    m_slots = a;
}

void DomUI::clearElementAuthor()
{
    m_children &= ~Author;
}

void DomUI::clearElementComment()
{
    m_children &= ~Comment;
}

void DomUI::clearElementExportMacro()
{
    m_children &= ~ExportMacro;
}

void DomUI::clearElementClass()
{
    m_children &= ~Class;
}

void DomUI::clearElementWidget()
{
    delete m_widget;
    m_widget = 0;
    m_children &= ~Widget;
}

void DomUI::clearElementLayoutDefault()
{
    delete m_layoutDefault;
    m_layoutDefault = 0;
    m_children &= ~LayoutDefault;
}

void DomUI::clearElementLayoutFunction()
{
    delete m_layoutFunction;
    m_layoutFunction = 0;
    m_children &= ~LayoutFunction;
}

void DomUI::clearElementPixmapFunction()
{
    m_children &= ~PixmapFunction;
}

void DomUI::clearElementCustomWidgets()
{
    delete m_customWidgets;
    m_customWidgets = 0;
    m_children &= ~CustomWidgets;
}

void DomUI::clearElementTabStops()
{
    delete m_tabStops;
    m_tabStops = 0;
    m_children &= ~TabStops;
}

void DomUI::clearElementImages()
{
    delete m_images;
    m_images = 0;
    m_children &= ~Images;
}

void DomUI::clearElementIncludes()
{
    delete m_includes;
    m_includes = 0;
    m_children &= ~Includes;
}

void DomUI::clearElementResources()
{
    delete m_resources;
    m_resources = 0;
    m_children &= ~Resources;
}

void DomUI::clearElementConnections()
{
    delete m_connections;
    m_connections = 0;
    m_children &= ~Connections;
}

void DomUI::clearElementDesignerdata()
{
    delete m_designerdata;
    m_designerdata = 0;
    m_children &= ~Designerdata;
}

void DomUI::clearElementSlots()
{
    delete m_slots;
    m_slots = 0;
    m_children &= ~Slots;
}

void DomIncludes::clear(bool clear_all)
{
    for (int i = 0; i < m_include.size(); ++i)
        delete m_include[i];
    m_include.clear();

    if (clear_all) {
    m_text = QString();
    }

}

DomIncludes::DomIncludes()
{
}

DomIncludes::~DomIncludes()
{
    for (int i = 0; i < m_include.size(); ++i)
        delete m_include[i];
    m_include.clear();
}

void DomIncludes::read(const QDomElement &node)
{

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("include")) {
            DomInclude *v = new DomInclude();
            v->read(e);
            m_include.append(v);
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomIncludes::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("includes") : tagName.toLower());

    QDomElement child;

    for (int i = 0; i < m_include.size(); ++i) {
        DomInclude* v = m_include[i];
        QDomNode child = v->write(doc, QLatin1String("include"));
        e.appendChild(child);
    }
    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomIncludes::setElementInclude(const QList<DomInclude*>& a)
{
    m_include = a;
}

void DomInclude::clear(bool clear_all)
{

    if (clear_all) {
    m_text = QString();
    m_has_attr_location = false;
    m_has_attr_impldecl = false;
    }

}

DomInclude::DomInclude()
{
    m_has_attr_location = false;
    m_has_attr_impldecl = false;
}

DomInclude::~DomInclude()
{
}

void DomInclude::read(const QDomElement &node)
{
    if (node.hasAttribute(QLatin1String("location")))
        setAttributeLocation(node.attribute(QLatin1String("location")));
    if (node.hasAttribute(QLatin1String("impldecl")))
        setAttributeImpldecl(node.attribute(QLatin1String("impldecl")));

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomInclude::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("include") : tagName.toLower());

    QDomElement child;

    if (hasAttributeLocation())
        e.setAttribute(QLatin1String("location"), attributeLocation());

    if (hasAttributeImpldecl())
        e.setAttribute(QLatin1String("impldecl"), attributeImpldecl());

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomResources::clear(bool clear_all)
{
    for (int i = 0; i < m_include.size(); ++i)
        delete m_include[i];
    m_include.clear();

    if (clear_all) {
    m_text = QString();
    m_has_attr_name = false;
    }

}

DomResources::DomResources()
{
    m_has_attr_name = false;
}

DomResources::~DomResources()
{
    for (int i = 0; i < m_include.size(); ++i)
        delete m_include[i];
    m_include.clear();
}

void DomResources::read(const QDomElement &node)
{
    if (node.hasAttribute(QLatin1String("name")))
        setAttributeName(node.attribute(QLatin1String("name")));

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("include")) {
            DomResource *v = new DomResource();
            v->read(e);
            m_include.append(v);
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomResources::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("resources") : tagName.toLower());

    QDomElement child;

    if (hasAttributeName())
        e.setAttribute(QLatin1String("name"), attributeName());

    for (int i = 0; i < m_include.size(); ++i) {
        DomResource* v = m_include[i];
        QDomNode child = v->write(doc, QLatin1String("include"));
        e.appendChild(child);
    }
    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomResources::setElementInclude(const QList<DomResource*>& a)
{
    m_include = a;
}

void DomResource::clear(bool clear_all)
{

    if (clear_all) {
    m_text = QString();
    m_has_attr_location = false;
    }

}

DomResource::DomResource()
{
    m_has_attr_location = false;
}

DomResource::~DomResource()
{
}

void DomResource::read(const QDomElement &node)
{
    if (node.hasAttribute(QLatin1String("location")))
        setAttributeLocation(node.attribute(QLatin1String("location")));

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomResource::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("resource") : tagName.toLower());

    QDomElement child;

    if (hasAttributeLocation())
        e.setAttribute(QLatin1String("location"), attributeLocation());

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomActionGroup::clear(bool clear_all)
{
    for (int i = 0; i < m_action.size(); ++i)
        delete m_action[i];
    m_action.clear();
    for (int i = 0; i < m_actionGroup.size(); ++i)
        delete m_actionGroup[i];
    m_actionGroup.clear();
    for (int i = 0; i < m_property.size(); ++i)
        delete m_property[i];
    m_property.clear();
    for (int i = 0; i < m_attribute.size(); ++i)
        delete m_attribute[i];
    m_attribute.clear();

    if (clear_all) {
    m_text = QString();
    m_has_attr_name = false;
    }

}

DomActionGroup::DomActionGroup()
{
    m_has_attr_name = false;
}

DomActionGroup::~DomActionGroup()
{
    for (int i = 0; i < m_action.size(); ++i)
        delete m_action[i];
    m_action.clear();
    for (int i = 0; i < m_actionGroup.size(); ++i)
        delete m_actionGroup[i];
    m_actionGroup.clear();
    for (int i = 0; i < m_property.size(); ++i)
        delete m_property[i];
    m_property.clear();
    for (int i = 0; i < m_attribute.size(); ++i)
        delete m_attribute[i];
    m_attribute.clear();
}

void DomActionGroup::read(const QDomElement &node)
{
    if (node.hasAttribute(QLatin1String("name")))
        setAttributeName(node.attribute(QLatin1String("name")));

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("action")) {
            DomAction *v = new DomAction();
            v->read(e);
            m_action.append(v);
            continue;
        }
        if (tag == QLatin1String("actiongroup")) {
            DomActionGroup *v = new DomActionGroup();
            v->read(e);
            m_actionGroup.append(v);
            continue;
        }
        if (tag == QLatin1String("property")) {
            DomProperty *v = new DomProperty();
            v->read(e);
            m_property.append(v);
            continue;
        }
        if (tag == QLatin1String("attribute")) {
            DomProperty *v = new DomProperty();
            v->read(e);
            m_attribute.append(v);
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomActionGroup::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("actiongroup") : tagName.toLower());

    QDomElement child;

    if (hasAttributeName())
        e.setAttribute(QLatin1String("name"), attributeName());

    for (int i = 0; i < m_action.size(); ++i) {
        DomAction* v = m_action[i];
        QDomNode child = v->write(doc, QLatin1String("action"));
        e.appendChild(child);
    }
    for (int i = 0; i < m_actionGroup.size(); ++i) {
        DomActionGroup* v = m_actionGroup[i];
        QDomNode child = v->write(doc, QLatin1String("actiongroup"));
        e.appendChild(child);
    }
    for (int i = 0; i < m_property.size(); ++i) {
        DomProperty* v = m_property[i];
        QDomNode child = v->write(doc, QLatin1String("property"));
        e.appendChild(child);
    }
    for (int i = 0; i < m_attribute.size(); ++i) {
        DomProperty* v = m_attribute[i];
        QDomNode child = v->write(doc, QLatin1String("attribute"));
        e.appendChild(child);
    }
    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomActionGroup::setElementAction(const QList<DomAction*>& a)
{
    m_action = a;
}

void DomActionGroup::setElementActionGroup(const QList<DomActionGroup*>& a)
{
    m_actionGroup = a;
}

void DomActionGroup::setElementProperty(const QList<DomProperty*>& a)
{
    m_property = a;
}

void DomActionGroup::setElementAttribute(const QList<DomProperty*>& a)
{
    m_attribute = a;
}

void DomAction::clear(bool clear_all)
{
    for (int i = 0; i < m_property.size(); ++i)
        delete m_property[i];
    m_property.clear();
    for (int i = 0; i < m_attribute.size(); ++i)
        delete m_attribute[i];
    m_attribute.clear();

    if (clear_all) {
    m_text = QString();
    m_has_attr_name = false;
    m_has_attr_menu = false;
    }

}

DomAction::DomAction()
{
    m_has_attr_name = false;
    m_has_attr_menu = false;
}

DomAction::~DomAction()
{
    for (int i = 0; i < m_property.size(); ++i)
        delete m_property[i];
    m_property.clear();
    for (int i = 0; i < m_attribute.size(); ++i)
        delete m_attribute[i];
    m_attribute.clear();
}

void DomAction::read(const QDomElement &node)
{
    if (node.hasAttribute(QLatin1String("name")))
        setAttributeName(node.attribute(QLatin1String("name")));
    if (node.hasAttribute(QLatin1String("menu")))
        setAttributeMenu(node.attribute(QLatin1String("menu")));

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("property")) {
            DomProperty *v = new DomProperty();
            v->read(e);
            m_property.append(v);
            continue;
        }
        if (tag == QLatin1String("attribute")) {
            DomProperty *v = new DomProperty();
            v->read(e);
            m_attribute.append(v);
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomAction::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("action") : tagName.toLower());

    QDomElement child;

    if (hasAttributeName())
        e.setAttribute(QLatin1String("name"), attributeName());

    if (hasAttributeMenu())
        e.setAttribute(QLatin1String("menu"), attributeMenu());

    for (int i = 0; i < m_property.size(); ++i) {
        DomProperty* v = m_property[i];
        QDomNode child = v->write(doc, QLatin1String("property"));
        e.appendChild(child);
    }
    for (int i = 0; i < m_attribute.size(); ++i) {
        DomProperty* v = m_attribute[i];
        QDomNode child = v->write(doc, QLatin1String("attribute"));
        e.appendChild(child);
    }
    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomAction::setElementProperty(const QList<DomProperty*>& a)
{
    m_property = a;
}

void DomAction::setElementAttribute(const QList<DomProperty*>& a)
{
    m_attribute = a;
}

void DomActionRef::clear(bool clear_all)
{

    if (clear_all) {
    m_text = QString();
    m_has_attr_name = false;
    }

}

DomActionRef::DomActionRef()
{
    m_has_attr_name = false;
}

DomActionRef::~DomActionRef()
{
}

void DomActionRef::read(const QDomElement &node)
{
    if (node.hasAttribute(QLatin1String("name")))
        setAttributeName(node.attribute(QLatin1String("name")));

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomActionRef::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("actionref") : tagName.toLower());

    QDomElement child;

    if (hasAttributeName())
        e.setAttribute(QLatin1String("name"), attributeName());

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomImages::clear(bool clear_all)
{
    for (int i = 0; i < m_image.size(); ++i)
        delete m_image[i];
    m_image.clear();

    if (clear_all) {
    m_text = QString();
    }

}

DomImages::DomImages()
{
}

DomImages::~DomImages()
{
    for (int i = 0; i < m_image.size(); ++i)
        delete m_image[i];
    m_image.clear();
}

void DomImages::read(const QDomElement &node)
{

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("image")) {
            DomImage *v = new DomImage();
            v->read(e);
            m_image.append(v);
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomImages::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("images") : tagName.toLower());

    QDomElement child;

    for (int i = 0; i < m_image.size(); ++i) {
        DomImage* v = m_image[i];
        QDomNode child = v->write(doc, QLatin1String("image"));
        e.appendChild(child);
    }
    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomImages::setElementImage(const QList<DomImage*>& a)
{
    m_image = a;
}

void DomImage::clear(bool clear_all)
{
    delete m_data;

    if (clear_all) {
    m_text = QString();
    m_has_attr_name = false;
    }

    m_children = 0;
    m_data = 0;
}

DomImage::DomImage()
{
    m_children = 0;
    m_has_attr_name = false;
    m_data = 0;
}

DomImage::~DomImage()
{
    delete m_data;
}

void DomImage::read(const QDomElement &node)
{
    if (node.hasAttribute(QLatin1String("name")))
        setAttributeName(node.attribute(QLatin1String("name")));

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("data")) {
            DomImageData *v = new DomImageData();
            v->read(e);
            setElementData(v);
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomImage::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("image") : tagName.toLower());

    QDomElement child;

    if (hasAttributeName())
        e.setAttribute(QLatin1String("name"), attributeName());

    if (m_children & Data) {
        e.appendChild(m_data->write(doc, QLatin1String("data")));
    }

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomImage::setElementData(DomImageData* a)
{
    delete m_data;
    m_children |= Data;
    m_data = a;
}

void DomImage::clearElementData()
{
    delete m_data;
    m_data = 0;
    m_children &= ~Data;
}

void DomImageData::clear(bool clear_all)
{

    if (clear_all) {
    m_text = QString();
    m_has_attr_format = false;
    m_has_attr_length = false;
    m_attr_length = 0;
    }

}

DomImageData::DomImageData()
{
    m_has_attr_format = false;
    m_has_attr_length = false;
    m_attr_length = 0;
}

DomImageData::~DomImageData()
{
}

void DomImageData::read(const QDomElement &node)
{
    if (node.hasAttribute(QLatin1String("format")))
        setAttributeFormat(node.attribute(QLatin1String("format")));
    if (node.hasAttribute(QLatin1String("length")))
        setAttributeLength(node.attribute(QLatin1String("length")).toInt());

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomImageData::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("imagedata") : tagName.toLower());

    QDomElement child;

    if (hasAttributeFormat())
        e.setAttribute(QLatin1String("format"), attributeFormat());

    if (hasAttributeLength())
        e.setAttribute(QLatin1String("length"), attributeLength());

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomCustomWidgets::clear(bool clear_all)
{
    for (int i = 0; i < m_customWidget.size(); ++i)
        delete m_customWidget[i];
    m_customWidget.clear();

    if (clear_all) {
    m_text = QString();
    }

}

DomCustomWidgets::DomCustomWidgets()
{
}

DomCustomWidgets::~DomCustomWidgets()
{
    for (int i = 0; i < m_customWidget.size(); ++i)
        delete m_customWidget[i];
    m_customWidget.clear();
}

void DomCustomWidgets::read(const QDomElement &node)
{

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("customwidget")) {
            DomCustomWidget *v = new DomCustomWidget();
            v->read(e);
            m_customWidget.append(v);
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomCustomWidgets::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("customwidgets") : tagName.toLower());

    QDomElement child;

    for (int i = 0; i < m_customWidget.size(); ++i) {
        DomCustomWidget* v = m_customWidget[i];
        QDomNode child = v->write(doc, QLatin1String("customwidget"));
        e.appendChild(child);
    }
    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomCustomWidgets::setElementCustomWidget(const QList<DomCustomWidget*>& a)
{
    m_customWidget = a;
}

void DomHeader::clear(bool clear_all)
{

    if (clear_all) {
    m_text = QString();
    m_has_attr_location = false;
    }

}

DomHeader::DomHeader()
{
    m_has_attr_location = false;
}

DomHeader::~DomHeader()
{
}

void DomHeader::read(const QDomElement &node)
{
    if (node.hasAttribute(QLatin1String("location")))
        setAttributeLocation(node.attribute(QLatin1String("location")));

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomHeader::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("header") : tagName.toLower());

    QDomElement child;

    if (hasAttributeLocation())
        e.setAttribute(QLatin1String("location"), attributeLocation());

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomCustomWidget::clear(bool clear_all)
{
    delete m_header;
    delete m_sizeHint;
    delete m_sizePolicy;
    delete m_script;
    delete m_properties;
    delete m_slots;

    if (clear_all) {
    m_text = QString();
    }

    m_children = 0;
    m_header = 0;
    m_sizeHint = 0;
    m_container = 0;
    m_sizePolicy = 0;
    m_script = 0;
    m_properties = 0;
    m_slots = 0;
}

DomCustomWidget::DomCustomWidget()
{
    m_children = 0;
    m_header = 0;
    m_sizeHint = 0;
    m_container = 0;
    m_sizePolicy = 0;
    m_script = 0;
    m_properties = 0;
    m_slots = 0;
}

DomCustomWidget::~DomCustomWidget()
{
    delete m_header;
    delete m_sizeHint;
    delete m_sizePolicy;
    delete m_script;
    delete m_properties;
    delete m_slots;
}

void DomCustomWidget::read(const QDomElement &node)
{

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("class")) {
            setElementClass(e.text());
            continue;
        }
        if (tag == QLatin1String("extends")) {
            setElementExtends(e.text());
            continue;
        }
        if (tag == QLatin1String("header")) {
            DomHeader *v = new DomHeader();
            v->read(e);
            setElementHeader(v);
            continue;
        }
        if (tag == QLatin1String("sizehint")) {
            DomSize *v = new DomSize();
            v->read(e);
            setElementSizeHint(v);
            continue;
        }
        if (tag == QLatin1String("addpagemethod")) {
            setElementAddPageMethod(e.text());
            continue;
        }
        if (tag == QLatin1String("container")) {
            setElementContainer(e.text().toInt());
            continue;
        }
        if (tag == QLatin1String("sizepolicy")) {
            DomSizePolicyData *v = new DomSizePolicyData();
            v->read(e);
            setElementSizePolicy(v);
            continue;
        }
        if (tag == QLatin1String("pixmap")) {
            setElementPixmap(e.text());
            continue;
        }
        if (tag == QLatin1String("script")) {
            DomScript *v = new DomScript();
            v->read(e);
            setElementScript(v);
            continue;
        }
        if (tag == QLatin1String("properties")) {
            DomProperties *v = new DomProperties();
            v->read(e);
            setElementProperties(v);
            continue;
        }
        if (tag == QLatin1String("slots")) {
            DomSlots *v = new DomSlots();
            v->read(e);
            setElementSlots(v);
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomCustomWidget::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("customwidget") : tagName.toLower());

    QDomElement child;

    if (m_children & Class) {
        child = doc.createElement(QLatin1String("class"));
        child.appendChild(doc.createTextNode(m_class));
        e.appendChild(child);
    }

    if (m_children & Extends) {
        child = doc.createElement(QLatin1String("extends"));
        child.appendChild(doc.createTextNode(m_extends));
        e.appendChild(child);
    }

    if (m_children & Header) {
        e.appendChild(m_header->write(doc, QLatin1String("header")));
    }

    if (m_children & SizeHint) {
        e.appendChild(m_sizeHint->write(doc, QLatin1String("sizehint")));
    }

    if (m_children & AddPageMethod) {
        child = doc.createElement(QLatin1String("addpagemethod"));
        child.appendChild(doc.createTextNode(m_addPageMethod));
        e.appendChild(child);
    }

    if (m_children & Container) {
        child = doc.createElement(QLatin1String("container"));
        child.appendChild(doc.createTextNode(QString::number(m_container)));
        e.appendChild(child);
    }

    if (m_children & SizePolicy) {
        e.appendChild(m_sizePolicy->write(doc, QLatin1String("sizepolicy")));
    }

    if (m_children & Pixmap) {
        child = doc.createElement(QLatin1String("pixmap"));
        child.appendChild(doc.createTextNode(m_pixmap));
        e.appendChild(child);
    }

    if (m_children & Script) {
        e.appendChild(m_script->write(doc, QLatin1String("script")));
    }

    if (m_children & Properties) {
        e.appendChild(m_properties->write(doc, QLatin1String("properties")));
    }

    if (m_children & Slots) {
        e.appendChild(m_slots->write(doc, QLatin1String("slots")));
    }

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomCustomWidget::setElementClass(const QString& a)
{
    m_children |= Class;
    m_class = a;
}

void DomCustomWidget::setElementExtends(const QString& a)
{
    m_children |= Extends;
    m_extends = a;
}

void DomCustomWidget::setElementHeader(DomHeader* a)
{
    delete m_header;
    m_children |= Header;
    m_header = a;
}

void DomCustomWidget::setElementSizeHint(DomSize* a)
{
    delete m_sizeHint;
    m_children |= SizeHint;
    m_sizeHint = a;
}

void DomCustomWidget::setElementAddPageMethod(const QString& a)
{
    m_children |= AddPageMethod;
    m_addPageMethod = a;
}

void DomCustomWidget::setElementContainer(int a)
{
    m_children |= Container;
    m_container = a;
}

void DomCustomWidget::setElementSizePolicy(DomSizePolicyData* a)
{
    delete m_sizePolicy;
    m_children |= SizePolicy;
    m_sizePolicy = a;
}

void DomCustomWidget::setElementPixmap(const QString& a)
{
    m_children |= Pixmap;
    m_pixmap = a;
}

void DomCustomWidget::setElementScript(DomScript* a)
{
    delete m_script;
    m_children |= Script;
    m_script = a;
}

void DomCustomWidget::setElementProperties(DomProperties* a)
{
    delete m_properties;
    m_children |= Properties;
    m_properties = a;
}

void DomCustomWidget::setElementSlots(DomSlots* a)
{
    delete m_slots;
    m_children |= Slots;
    m_slots = a;
}

void DomCustomWidget::clearElementClass()
{
    m_children &= ~Class;
}

void DomCustomWidget::clearElementExtends()
{
    m_children &= ~Extends;
}

void DomCustomWidget::clearElementHeader()
{
    delete m_header;
    m_header = 0;
    m_children &= ~Header;
}

void DomCustomWidget::clearElementSizeHint()
{
    delete m_sizeHint;
    m_sizeHint = 0;
    m_children &= ~SizeHint;
}

void DomCustomWidget::clearElementAddPageMethod()
{
    m_children &= ~AddPageMethod;
}

void DomCustomWidget::clearElementContainer()
{
    m_children &= ~Container;
}

void DomCustomWidget::clearElementSizePolicy()
{
    delete m_sizePolicy;
    m_sizePolicy = 0;
    m_children &= ~SizePolicy;
}

void DomCustomWidget::clearElementPixmap()
{
    m_children &= ~Pixmap;
}

void DomCustomWidget::clearElementScript()
{
    delete m_script;
    m_script = 0;
    m_children &= ~Script;
}

void DomCustomWidget::clearElementProperties()
{
    delete m_properties;
    m_properties = 0;
    m_children &= ~Properties;
}

void DomCustomWidget::clearElementSlots()
{
    delete m_slots;
    m_slots = 0;
    m_children &= ~Slots;
}

void DomProperties::clear(bool clear_all)
{
    for (int i = 0; i < m_property.size(); ++i)
        delete m_property[i];
    m_property.clear();

    if (clear_all) {
    m_text = QString();
    }

}

DomProperties::DomProperties()
{
}

DomProperties::~DomProperties()
{
    for (int i = 0; i < m_property.size(); ++i)
        delete m_property[i];
    m_property.clear();
}

void DomProperties::read(const QDomElement &node)
{

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("property")) {
            DomPropertyData *v = new DomPropertyData();
            v->read(e);
            m_property.append(v);
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomProperties::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("properties") : tagName.toLower());

    QDomElement child;

    for (int i = 0; i < m_property.size(); ++i) {
        DomPropertyData* v = m_property[i];
        QDomNode child = v->write(doc, QLatin1String("property"));
        e.appendChild(child);
    }
    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomProperties::setElementProperty(const QList<DomPropertyData*>& a)
{
    m_property = a;
}

void DomPropertyData::clear(bool clear_all)
{

    if (clear_all) {
    m_text = QString();
    m_has_attr_type = false;
    }

}

DomPropertyData::DomPropertyData()
{
    m_has_attr_type = false;
}

DomPropertyData::~DomPropertyData()
{
}

void DomPropertyData::read(const QDomElement &node)
{
    if (node.hasAttribute(QLatin1String("type")))
        setAttributeType(node.attribute(QLatin1String("type")));

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomPropertyData::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("propertydata") : tagName.toLower());

    QDomElement child;

    if (hasAttributeType())
        e.setAttribute(QLatin1String("type"), attributeType());

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomSizePolicyData::clear(bool clear_all)
{

    if (clear_all) {
    m_text = QString();
    }

    m_children = 0;
    m_horData = 0;
    m_verData = 0;
}

DomSizePolicyData::DomSizePolicyData()
{
    m_children = 0;
    m_horData = 0;
    m_verData = 0;
}

DomSizePolicyData::~DomSizePolicyData()
{
}

void DomSizePolicyData::read(const QDomElement &node)
{

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("hordata")) {
            setElementHorData(e.text().toInt());
            continue;
        }
        if (tag == QLatin1String("verdata")) {
            setElementVerData(e.text().toInt());
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomSizePolicyData::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("sizepolicydata") : tagName.toLower());

    QDomElement child;

    if (m_children & HorData) {
        child = doc.createElement(QLatin1String("hordata"));
        child.appendChild(doc.createTextNode(QString::number(m_horData)));
        e.appendChild(child);
    }

    if (m_children & VerData) {
        child = doc.createElement(QLatin1String("verdata"));
        child.appendChild(doc.createTextNode(QString::number(m_verData)));
        e.appendChild(child);
    }

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomSizePolicyData::setElementHorData(int a)
{
    m_children |= HorData;
    m_horData = a;
}

void DomSizePolicyData::setElementVerData(int a)
{
    m_children |= VerData;
    m_verData = a;
}

void DomSizePolicyData::clearElementHorData()
{
    m_children &= ~HorData;
}

void DomSizePolicyData::clearElementVerData()
{
    m_children &= ~VerData;
}

void DomLayoutDefault::clear(bool clear_all)
{

    if (clear_all) {
    m_text = QString();
    m_has_attr_spacing = false;
    m_attr_spacing = 0;
    m_has_attr_margin = false;
    m_attr_margin = 0;
    }

}

DomLayoutDefault::DomLayoutDefault()
{
    m_has_attr_spacing = false;
    m_attr_spacing = 0;
    m_has_attr_margin = false;
    m_attr_margin = 0;
}

DomLayoutDefault::~DomLayoutDefault()
{
}

void DomLayoutDefault::read(const QDomElement &node)
{
    if (node.hasAttribute(QLatin1String("spacing")))
        setAttributeSpacing(node.attribute(QLatin1String("spacing")).toInt());
    if (node.hasAttribute(QLatin1String("margin")))
        setAttributeMargin(node.attribute(QLatin1String("margin")).toInt());

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomLayoutDefault::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("layoutdefault") : tagName.toLower());

    QDomElement child;

    if (hasAttributeSpacing())
        e.setAttribute(QLatin1String("spacing"), attributeSpacing());

    if (hasAttributeMargin())
        e.setAttribute(QLatin1String("margin"), attributeMargin());

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomLayoutFunction::clear(bool clear_all)
{

    if (clear_all) {
    m_text = QString();
    m_has_attr_spacing = false;
    m_has_attr_margin = false;
    }

}

DomLayoutFunction::DomLayoutFunction()
{
    m_has_attr_spacing = false;
    m_has_attr_margin = false;
}

DomLayoutFunction::~DomLayoutFunction()
{
}

void DomLayoutFunction::read(const QDomElement &node)
{
    if (node.hasAttribute(QLatin1String("spacing")))
        setAttributeSpacing(node.attribute(QLatin1String("spacing")));
    if (node.hasAttribute(QLatin1String("margin")))
        setAttributeMargin(node.attribute(QLatin1String("margin")));

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomLayoutFunction::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("layoutfunction") : tagName.toLower());

    QDomElement child;

    if (hasAttributeSpacing())
        e.setAttribute(QLatin1String("spacing"), attributeSpacing());

    if (hasAttributeMargin())
        e.setAttribute(QLatin1String("margin"), attributeMargin());

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomTabStops::clear(bool clear_all)
{
    m_tabStop.clear();

    if (clear_all) {
    m_text = QString();
    }

}

DomTabStops::DomTabStops()
{
}

DomTabStops::~DomTabStops()
{
    m_tabStop.clear();
}

void DomTabStops::read(const QDomElement &node)
{

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("tabstop")) {
            m_tabStop.append(e.text());
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomTabStops::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("tabstops") : tagName.toLower());

    QDomElement child;

    for (int i = 0; i < m_tabStop.size(); ++i) {
        QString v = m_tabStop[i];
        QDomNode child = doc.createElement(QLatin1String("tabstop"));
        child.appendChild(doc.createTextNode(v));
        e.appendChild(child);
    }
    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomTabStops::setElementTabStop(const QStringList& a)
{
    m_tabStop = a;
}

void DomLayout::clear(bool clear_all)
{
    for (int i = 0; i < m_property.size(); ++i)
        delete m_property[i];
    m_property.clear();
    for (int i = 0; i < m_attribute.size(); ++i)
        delete m_attribute[i];
    m_attribute.clear();
    for (int i = 0; i < m_item.size(); ++i)
        delete m_item[i];
    m_item.clear();

    if (clear_all) {
    m_text = QString();
    m_has_attr_class = false;
    m_has_attr_name = false;
    }

}

DomLayout::DomLayout()
{
    m_has_attr_class = false;
    m_has_attr_name = false;
}

DomLayout::~DomLayout()
{
    for (int i = 0; i < m_property.size(); ++i)
        delete m_property[i];
    m_property.clear();
    for (int i = 0; i < m_attribute.size(); ++i)
        delete m_attribute[i];
    m_attribute.clear();
    for (int i = 0; i < m_item.size(); ++i)
        delete m_item[i];
    m_item.clear();
}

void DomLayout::read(const QDomElement &node)
{
    if (node.hasAttribute(QLatin1String("class")))
        setAttributeClass(node.attribute(QLatin1String("class")));
    if (node.hasAttribute(QLatin1String("name")))
        setAttributeName(node.attribute(QLatin1String("name")));

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("property")) {
            DomProperty *v = new DomProperty();
            v->read(e);
            m_property.append(v);
            continue;
        }
        if (tag == QLatin1String("attribute")) {
            DomProperty *v = new DomProperty();
            v->read(e);
            m_attribute.append(v);
            continue;
        }
        if (tag == QLatin1String("item")) {
            DomLayoutItem *v = new DomLayoutItem();
            v->read(e);
            m_item.append(v);
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomLayout::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("layout") : tagName.toLower());

    QDomElement child;

    if (hasAttributeClass())
        e.setAttribute(QLatin1String("class"), attributeClass());

    if (hasAttributeName())
        e.setAttribute(QLatin1String("name"), attributeName());

    for (int i = 0; i < m_property.size(); ++i) {
        DomProperty* v = m_property[i];
        QDomNode child = v->write(doc, QLatin1String("property"));
        e.appendChild(child);
    }
    for (int i = 0; i < m_attribute.size(); ++i) {
        DomProperty* v = m_attribute[i];
        QDomNode child = v->write(doc, QLatin1String("attribute"));
        e.appendChild(child);
    }
    for (int i = 0; i < m_item.size(); ++i) {
        DomLayoutItem* v = m_item[i];
        QDomNode child = v->write(doc, QLatin1String("item"));
        e.appendChild(child);
    }
    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomLayout::setElementProperty(const QList<DomProperty*>& a)
{
    m_property = a;
}

void DomLayout::setElementAttribute(const QList<DomProperty*>& a)
{
    m_attribute = a;
}

void DomLayout::setElementItem(const QList<DomLayoutItem*>& a)
{
    m_item = a;
}

void DomLayoutItem::clear(bool clear_all)
{
    delete m_widget;
    delete m_layout;
    delete m_spacer;

    if (clear_all) {
    m_text = QString();
    m_has_attr_row = false;
    m_attr_row = 0;
    m_has_attr_column = false;
    m_attr_column = 0;
    m_has_attr_rowSpan = false;
    m_attr_rowSpan = 0;
    m_has_attr_colSpan = false;
    m_attr_colSpan = 0;
    }

    m_kind = Unknown;

    m_widget = 0;
    m_layout = 0;
    m_spacer = 0;
}

DomLayoutItem::DomLayoutItem()
{
    m_kind = Unknown;

    m_has_attr_row = false;
    m_attr_row = 0;
    m_has_attr_column = false;
    m_attr_column = 0;
    m_has_attr_rowSpan = false;
    m_attr_rowSpan = 0;
    m_has_attr_colSpan = false;
    m_attr_colSpan = 0;
    m_widget = 0;
    m_layout = 0;
    m_spacer = 0;
}

DomLayoutItem::~DomLayoutItem()
{
    delete m_widget;
    delete m_layout;
    delete m_spacer;
}

void DomLayoutItem::read(const QDomElement &node)
{
    if (node.hasAttribute(QLatin1String("row")))
        setAttributeRow(node.attribute(QLatin1String("row")).toInt());
    if (node.hasAttribute(QLatin1String("column")))
        setAttributeColumn(node.attribute(QLatin1String("column")).toInt());
    if (node.hasAttribute(QLatin1String("rowspan")))
        setAttributeRowSpan(node.attribute(QLatin1String("rowspan")).toInt());
    if (node.hasAttribute(QLatin1String("colspan")))
        setAttributeColSpan(node.attribute(QLatin1String("colspan")).toInt());

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("widget")) {
            DomWidget *v = new DomWidget();
            v->read(e);
            setElementWidget(v);
            continue;
        }
        if (tag == QLatin1String("layout")) {
            DomLayout *v = new DomLayout();
            v->read(e);
            setElementLayout(v);
            continue;
        }
        if (tag == QLatin1String("spacer")) {
            DomSpacer *v = new DomSpacer();
            v->read(e);
            setElementSpacer(v);
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomLayoutItem::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("layoutitem") : tagName.toLower());

    QDomElement child;

    if (hasAttributeRow())
        e.setAttribute(QLatin1String("row"), attributeRow());

    if (hasAttributeColumn())
        e.setAttribute(QLatin1String("column"), attributeColumn());

    if (hasAttributeRowSpan())
        e.setAttribute(QLatin1String("rowspan"), attributeRowSpan());

    if (hasAttributeColSpan())
        e.setAttribute(QLatin1String("colspan"), attributeColSpan());

    switch(kind()) {
        case Widget: {
            DomWidget* v = elementWidget();
            if (v != 0) {
                QDomElement child = v->write(doc, QLatin1String("widget"));
                e.appendChild(child);
            }
            break;
        }
        case Layout: {
            DomLayout* v = elementLayout();
            if (v != 0) {
                QDomElement child = v->write(doc, QLatin1String("layout"));
                e.appendChild(child);
            }
            break;
        }
        case Spacer: {
            DomSpacer* v = elementSpacer();
            if (v != 0) {
                QDomElement child = v->write(doc, QLatin1String("spacer"));
                e.appendChild(child);
            }
            break;
        }
        default:
            break;
    }
    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomLayoutItem::setElementWidget(DomWidget* a)
{
    clear(false);
    m_kind = Widget;
    m_widget = a;
}

void DomLayoutItem::setElementLayout(DomLayout* a)
{
    clear(false);
    m_kind = Layout;
    m_layout = a;
}

void DomLayoutItem::setElementSpacer(DomSpacer* a)
{
    clear(false);
    m_kind = Spacer;
    m_spacer = a;
}

void DomRow::clear(bool clear_all)
{
    for (int i = 0; i < m_property.size(); ++i)
        delete m_property[i];
    m_property.clear();

    if (clear_all) {
    m_text = QString();
    }

}

DomRow::DomRow()
{
}

DomRow::~DomRow()
{
    for (int i = 0; i < m_property.size(); ++i)
        delete m_property[i];
    m_property.clear();
}

void DomRow::read(const QDomElement &node)
{

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("property")) {
            DomProperty *v = new DomProperty();
            v->read(e);
            m_property.append(v);
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomRow::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("row") : tagName.toLower());

    QDomElement child;

    for (int i = 0; i < m_property.size(); ++i) {
        DomProperty* v = m_property[i];
        QDomNode child = v->write(doc, QLatin1String("property"));
        e.appendChild(child);
    }
    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomRow::setElementProperty(const QList<DomProperty*>& a)
{
    m_property = a;
}

void DomColumn::clear(bool clear_all)
{
    for (int i = 0; i < m_property.size(); ++i)
        delete m_property[i];
    m_property.clear();

    if (clear_all) {
    m_text = QString();
    }

}

DomColumn::DomColumn()
{
}

DomColumn::~DomColumn()
{
    for (int i = 0; i < m_property.size(); ++i)
        delete m_property[i];
    m_property.clear();
}

void DomColumn::read(const QDomElement &node)
{

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("property")) {
            DomProperty *v = new DomProperty();
            v->read(e);
            m_property.append(v);
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomColumn::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("column") : tagName.toLower());

    QDomElement child;

    for (int i = 0; i < m_property.size(); ++i) {
        DomProperty* v = m_property[i];
        QDomNode child = v->write(doc, QLatin1String("property"));
        e.appendChild(child);
    }
    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomColumn::setElementProperty(const QList<DomProperty*>& a)
{
    m_property = a;
}

void DomItem::clear(bool clear_all)
{
    for (int i = 0; i < m_property.size(); ++i)
        delete m_property[i];
    m_property.clear();
    for (int i = 0; i < m_item.size(); ++i)
        delete m_item[i];
    m_item.clear();

    if (clear_all) {
    m_text = QString();
    m_has_attr_row = false;
    m_attr_row = 0;
    m_has_attr_column = false;
    m_attr_column = 0;
    }

}

DomItem::DomItem()
{
    m_has_attr_row = false;
    m_attr_row = 0;
    m_has_attr_column = false;
    m_attr_column = 0;
}

DomItem::~DomItem()
{
    for (int i = 0; i < m_property.size(); ++i)
        delete m_property[i];
    m_property.clear();
    for (int i = 0; i < m_item.size(); ++i)
        delete m_item[i];
    m_item.clear();
}

void DomItem::read(const QDomElement &node)
{
    if (node.hasAttribute(QLatin1String("row")))
        setAttributeRow(node.attribute(QLatin1String("row")).toInt());
    if (node.hasAttribute(QLatin1String("column")))
        setAttributeColumn(node.attribute(QLatin1String("column")).toInt());

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("property")) {
            DomProperty *v = new DomProperty();
            v->read(e);
            m_property.append(v);
            continue;
        }
        if (tag == QLatin1String("item")) {
            DomItem *v = new DomItem();
            v->read(e);
            m_item.append(v);
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomItem::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("item") : tagName.toLower());

    QDomElement child;

    if (hasAttributeRow())
        e.setAttribute(QLatin1String("row"), attributeRow());

    if (hasAttributeColumn())
        e.setAttribute(QLatin1String("column"), attributeColumn());

    for (int i = 0; i < m_property.size(); ++i) {
        DomProperty* v = m_property[i];
        QDomNode child = v->write(doc, QLatin1String("property"));
        e.appendChild(child);
    }
    for (int i = 0; i < m_item.size(); ++i) {
        DomItem* v = m_item[i];
        QDomNode child = v->write(doc, QLatin1String("item"));
        e.appendChild(child);
    }
    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomItem::setElementProperty(const QList<DomProperty*>& a)
{
    m_property = a;
}

void DomItem::setElementItem(const QList<DomItem*>& a)
{
    m_item = a;
}

void DomWidget::clear(bool clear_all)
{
    m_class.clear();
    for (int i = 0; i < m_property.size(); ++i)
        delete m_property[i];
    m_property.clear();
    for (int i = 0; i < m_script.size(); ++i)
        delete m_script[i];
    m_script.clear();
    for (int i = 0; i < m_widgetData.size(); ++i)
        delete m_widgetData[i];
    m_widgetData.clear();
    for (int i = 0; i < m_attribute.size(); ++i)
        delete m_attribute[i];
    m_attribute.clear();
    for (int i = 0; i < m_row.size(); ++i)
        delete m_row[i];
    m_row.clear();
    for (int i = 0; i < m_column.size(); ++i)
        delete m_column[i];
    m_column.clear();
    for (int i = 0; i < m_item.size(); ++i)
        delete m_item[i];
    m_item.clear();
    for (int i = 0; i < m_layout.size(); ++i)
        delete m_layout[i];
    m_layout.clear();
    for (int i = 0; i < m_widget.size(); ++i)
        delete m_widget[i];
    m_widget.clear();
    for (int i = 0; i < m_action.size(); ++i)
        delete m_action[i];
    m_action.clear();
    for (int i = 0; i < m_actionGroup.size(); ++i)
        delete m_actionGroup[i];
    m_actionGroup.clear();
    for (int i = 0; i < m_addAction.size(); ++i)
        delete m_addAction[i];
    m_addAction.clear();
    m_zOrder.clear();

    if (clear_all) {
    m_text = QString();
    m_has_attr_class = false;
    m_has_attr_name = false;
    m_has_attr_native = false;
    m_attr_native = false;
    }

}

DomWidget::DomWidget()
{
    m_has_attr_class = false;
    m_has_attr_name = false;
    m_has_attr_native = false;
    m_attr_native = false;
}

DomWidget::~DomWidget()
{
    m_class.clear();
    for (int i = 0; i < m_property.size(); ++i)
        delete m_property[i];
    m_property.clear();
    for (int i = 0; i < m_script.size(); ++i)
        delete m_script[i];
    m_script.clear();
    for (int i = 0; i < m_widgetData.size(); ++i)
        delete m_widgetData[i];
    m_widgetData.clear();
    for (int i = 0; i < m_attribute.size(); ++i)
        delete m_attribute[i];
    m_attribute.clear();
    for (int i = 0; i < m_row.size(); ++i)
        delete m_row[i];
    m_row.clear();
    for (int i = 0; i < m_column.size(); ++i)
        delete m_column[i];
    m_column.clear();
    for (int i = 0; i < m_item.size(); ++i)
        delete m_item[i];
    m_item.clear();
    for (int i = 0; i < m_layout.size(); ++i)
        delete m_layout[i];
    m_layout.clear();
    for (int i = 0; i < m_widget.size(); ++i)
        delete m_widget[i];
    m_widget.clear();
    for (int i = 0; i < m_action.size(); ++i)
        delete m_action[i];
    m_action.clear();
    for (int i = 0; i < m_actionGroup.size(); ++i)
        delete m_actionGroup[i];
    m_actionGroup.clear();
    for (int i = 0; i < m_addAction.size(); ++i)
        delete m_addAction[i];
    m_addAction.clear();
    m_zOrder.clear();
}

void DomWidget::read(const QDomElement &node)
{
    if (node.hasAttribute(QLatin1String("class")))
        setAttributeClass(node.attribute(QLatin1String("class")));
    if (node.hasAttribute(QLatin1String("name")))
        setAttributeName(node.attribute(QLatin1String("name")));
    if (node.hasAttribute(QLatin1String("native")))
        setAttributeNative((node.attribute(QLatin1String("native")) == QLatin1String("true") ? true : false));

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("class")) {
            m_class.append(e.text());
            continue;
        }
        if (tag == QLatin1String("property")) {
            DomProperty *v = new DomProperty();
            v->read(e);
            m_property.append(v);
            continue;
        }
        if (tag == QLatin1String("script")) {
            DomScript *v = new DomScript();
            v->read(e);
            m_script.append(v);
            continue;
        }
        if (tag == QLatin1String("widgetdata")) {
            DomWidgetData *v = new DomWidgetData();
            v->read(e);
            m_widgetData.append(v);
            continue;
        }
        if (tag == QLatin1String("attribute")) {
            DomProperty *v = new DomProperty();
            v->read(e);
            m_attribute.append(v);
            continue;
        }
        if (tag == QLatin1String("row")) {
            DomRow *v = new DomRow();
            v->read(e);
            m_row.append(v);
            continue;
        }
        if (tag == QLatin1String("column")) {
            DomColumn *v = new DomColumn();
            v->read(e);
            m_column.append(v);
            continue;
        }
        if (tag == QLatin1String("item")) {
            DomItem *v = new DomItem();
            v->read(e);
            m_item.append(v);
            continue;
        }
        if (tag == QLatin1String("layout")) {
            DomLayout *v = new DomLayout();
            v->read(e);
            m_layout.append(v);
            continue;
        }
        if (tag == QLatin1String("widget")) {
            DomWidget *v = new DomWidget();
            v->read(e);
            m_widget.append(v);
            continue;
        }
        if (tag == QLatin1String("action")) {
            DomAction *v = new DomAction();
            v->read(e);
            m_action.append(v);
            continue;
        }
        if (tag == QLatin1String("actiongroup")) {
            DomActionGroup *v = new DomActionGroup();
            v->read(e);
            m_actionGroup.append(v);
            continue;
        }
        if (tag == QLatin1String("addaction")) {
            DomActionRef *v = new DomActionRef();
            v->read(e);
            m_addAction.append(v);
            continue;
        }
        if (tag == QLatin1String("zorder")) {
            m_zOrder.append(e.text());
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomWidget::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("widget") : tagName.toLower());

    QDomElement child;

    if (hasAttributeClass())
        e.setAttribute(QLatin1String("class"), attributeClass());

    if (hasAttributeName())
        e.setAttribute(QLatin1String("name"), attributeName());

    if (hasAttributeNative())
        e.setAttribute(QLatin1String("native"), attributeNative());

    for (int i = 0; i < m_class.size(); ++i) {
        QString v = m_class[i];
        QDomNode child = doc.createElement(QLatin1String("class"));
        child.appendChild(doc.createTextNode(v));
        e.appendChild(child);
    }
    for (int i = 0; i < m_property.size(); ++i) {
        DomProperty* v = m_property[i];
        QDomNode child = v->write(doc, QLatin1String("property"));
        e.appendChild(child);
    }
    for (int i = 0; i < m_script.size(); ++i) {
        DomScript* v = m_script[i];
        QDomNode child = v->write(doc, QLatin1String("script"));
        e.appendChild(child);
    }
    for (int i = 0; i < m_widgetData.size(); ++i) {
        DomWidgetData* v = m_widgetData[i];
        QDomNode child = v->write(doc, QLatin1String("widgetdata"));
        e.appendChild(child);
    }
    for (int i = 0; i < m_attribute.size(); ++i) {
        DomProperty* v = m_attribute[i];
        QDomNode child = v->write(doc, QLatin1String("attribute"));
        e.appendChild(child);
    }
    for (int i = 0; i < m_row.size(); ++i) {
        DomRow* v = m_row[i];
        QDomNode child = v->write(doc, QLatin1String("row"));
        e.appendChild(child);
    }
    for (int i = 0; i < m_column.size(); ++i) {
        DomColumn* v = m_column[i];
        QDomNode child = v->write(doc, QLatin1String("column"));
        e.appendChild(child);
    }
    for (int i = 0; i < m_item.size(); ++i) {
        DomItem* v = m_item[i];
        QDomNode child = v->write(doc, QLatin1String("item"));
        e.appendChild(child);
    }
    for (int i = 0; i < m_layout.size(); ++i) {
        DomLayout* v = m_layout[i];
        QDomNode child = v->write(doc, QLatin1String("layout"));
        e.appendChild(child);
    }
    for (int i = 0; i < m_widget.size(); ++i) {
        DomWidget* v = m_widget[i];
        QDomNode child = v->write(doc, QLatin1String("widget"));
        e.appendChild(child);
    }
    for (int i = 0; i < m_action.size(); ++i) {
        DomAction* v = m_action[i];
        QDomNode child = v->write(doc, QLatin1String("action"));
        e.appendChild(child);
    }
    for (int i = 0; i < m_actionGroup.size(); ++i) {
        DomActionGroup* v = m_actionGroup[i];
        QDomNode child = v->write(doc, QLatin1String("actiongroup"));
        e.appendChild(child);
    }
    for (int i = 0; i < m_addAction.size(); ++i) {
        DomActionRef* v = m_addAction[i];
        QDomNode child = v->write(doc, QLatin1String("addaction"));
        e.appendChild(child);
    }
    for (int i = 0; i < m_zOrder.size(); ++i) {
        QString v = m_zOrder[i];
        QDomNode child = doc.createElement(QLatin1String("zorder"));
        child.appendChild(doc.createTextNode(v));
        e.appendChild(child);
    }
    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomWidget::setElementClass(const QStringList& a)
{
    m_class = a;
}

void DomWidget::setElementProperty(const QList<DomProperty*>& a)
{
    m_property = a;
}

void DomWidget::setElementScript(const QList<DomScript*>& a)
{
    m_script = a;
}

void DomWidget::setElementWidgetData(const QList<DomWidgetData*>& a)
{
    m_widgetData = a;
}

void DomWidget::setElementAttribute(const QList<DomProperty*>& a)
{
    m_attribute = a;
}

void DomWidget::setElementRow(const QList<DomRow*>& a)
{
    m_row = a;
}

void DomWidget::setElementColumn(const QList<DomColumn*>& a)
{
    m_column = a;
}

void DomWidget::setElementItem(const QList<DomItem*>& a)
{
    m_item = a;
}

void DomWidget::setElementLayout(const QList<DomLayout*>& a)
{
    m_layout = a;
}

void DomWidget::setElementWidget(const QList<DomWidget*>& a)
{
    m_widget = a;
}

void DomWidget::setElementAction(const QList<DomAction*>& a)
{
    m_action = a;
}

void DomWidget::setElementActionGroup(const QList<DomActionGroup*>& a)
{
    m_actionGroup = a;
}

void DomWidget::setElementAddAction(const QList<DomActionRef*>& a)
{
    m_addAction = a;
}

void DomWidget::setElementZOrder(const QStringList& a)
{
    m_zOrder = a;
}

void DomSpacer::clear(bool clear_all)
{
    for (int i = 0; i < m_property.size(); ++i)
        delete m_property[i];
    m_property.clear();

    if (clear_all) {
    m_text = QString();
    m_has_attr_name = false;
    }

}

DomSpacer::DomSpacer()
{
    m_has_attr_name = false;
}

DomSpacer::~DomSpacer()
{
    for (int i = 0; i < m_property.size(); ++i)
        delete m_property[i];
    m_property.clear();
}

void DomSpacer::read(const QDomElement &node)
{
    if (node.hasAttribute(QLatin1String("name")))
        setAttributeName(node.attribute(QLatin1String("name")));

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("property")) {
            DomProperty *v = new DomProperty();
            v->read(e);
            m_property.append(v);
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomSpacer::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("spacer") : tagName.toLower());

    QDomElement child;

    if (hasAttributeName())
        e.setAttribute(QLatin1String("name"), attributeName());

    for (int i = 0; i < m_property.size(); ++i) {
        DomProperty* v = m_property[i];
        QDomNode child = v->write(doc, QLatin1String("property"));
        e.appendChild(child);
    }
    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomSpacer::setElementProperty(const QList<DomProperty*>& a)
{
    m_property = a;
}

void DomColor::clear(bool clear_all)
{

    if (clear_all) {
    m_text = QString();
    m_has_attr_alpha = false;
    m_attr_alpha = 0;
    }

    m_children = 0;
    m_red = 0;
    m_green = 0;
    m_blue = 0;
}

DomColor::DomColor()
{
    m_children = 0;
    m_has_attr_alpha = false;
    m_attr_alpha = 0;
    m_red = 0;
    m_green = 0;
    m_blue = 0;
}

DomColor::~DomColor()
{
}

void DomColor::read(const QDomElement &node)
{
    if (node.hasAttribute(QLatin1String("alpha")))
        setAttributeAlpha(node.attribute(QLatin1String("alpha")).toInt());

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("red")) {
            setElementRed(e.text().toInt());
            continue;
        }
        if (tag == QLatin1String("green")) {
            setElementGreen(e.text().toInt());
            continue;
        }
        if (tag == QLatin1String("blue")) {
            setElementBlue(e.text().toInt());
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomColor::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("color") : tagName.toLower());

    QDomElement child;

    if (hasAttributeAlpha())
        e.setAttribute(QLatin1String("alpha"), attributeAlpha());

    if (m_children & Red) {
        child = doc.createElement(QLatin1String("red"));
        child.appendChild(doc.createTextNode(QString::number(m_red)));
        e.appendChild(child);
    }

    if (m_children & Green) {
        child = doc.createElement(QLatin1String("green"));
        child.appendChild(doc.createTextNode(QString::number(m_green)));
        e.appendChild(child);
    }

    if (m_children & Blue) {
        child = doc.createElement(QLatin1String("blue"));
        child.appendChild(doc.createTextNode(QString::number(m_blue)));
        e.appendChild(child);
    }

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomColor::setElementRed(int a)
{
    m_children |= Red;
    m_red = a;
}

void DomColor::setElementGreen(int a)
{
    m_children |= Green;
    m_green = a;
}

void DomColor::setElementBlue(int a)
{
    m_children |= Blue;
    m_blue = a;
}

void DomColor::clearElementRed()
{
    m_children &= ~Red;
}

void DomColor::clearElementGreen()
{
    m_children &= ~Green;
}

void DomColor::clearElementBlue()
{
    m_children &= ~Blue;
}

void DomGradientStop::clear(bool clear_all)
{
    delete m_color;

    if (clear_all) {
    m_text = QString();
    m_has_attr_position = false;
    m_attr_position = 0.0;
    }

    m_children = 0;
    m_color = 0;
}

DomGradientStop::DomGradientStop()
{
    m_children = 0;
    m_has_attr_position = false;
    m_attr_position = 0.0;
    m_color = 0;
}

DomGradientStop::~DomGradientStop()
{
    delete m_color;
}

void DomGradientStop::read(const QDomElement &node)
{
    if (node.hasAttribute(QLatin1String("position")))
        setAttributePosition(node.attribute(QLatin1String("position")).toDouble());

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("color")) {
            DomColor *v = new DomColor();
            v->read(e);
            setElementColor(v);
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomGradientStop::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("gradientstop") : tagName.toLower());

    QDomElement child;

    if (hasAttributePosition())
        e.setAttribute(QLatin1String("position"), attributePosition());

    if (m_children & Color) {
        e.appendChild(m_color->write(doc, QLatin1String("color")));
    }

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomGradientStop::setElementColor(DomColor* a)
{
    delete m_color;
    m_children |= Color;
    m_color = a;
}

void DomGradientStop::clearElementColor()
{
    delete m_color;
    m_color = 0;
    m_children &= ~Color;
}

void DomGradient::clear(bool clear_all)
{
    for (int i = 0; i < m_gradientStop.size(); ++i)
        delete m_gradientStop[i];
    m_gradientStop.clear();

    if (clear_all) {
    m_text = QString();
    m_has_attr_startX = false;
    m_attr_startX = 0.0;
    m_has_attr_startY = false;
    m_attr_startY = 0.0;
    m_has_attr_endX = false;
    m_attr_endX = 0.0;
    m_has_attr_endY = false;
    m_attr_endY = 0.0;
    m_has_attr_centralX = false;
    m_attr_centralX = 0.0;
    m_has_attr_centralY = false;
    m_attr_centralY = 0.0;
    m_has_attr_focalX = false;
    m_attr_focalX = 0.0;
    m_has_attr_focalY = false;
    m_attr_focalY = 0.0;
    m_has_attr_radius = false;
    m_attr_radius = 0.0;
    m_has_attr_angle = false;
    m_attr_angle = 0.0;
    m_has_attr_type = false;
    m_has_attr_spread = false;
    m_has_attr_coordinateMode = false;
    }

}

DomGradient::DomGradient()
{
    m_has_attr_startX = false;
    m_attr_startX = 0.0;
    m_has_attr_startY = false;
    m_attr_startY = 0.0;
    m_has_attr_endX = false;
    m_attr_endX = 0.0;
    m_has_attr_endY = false;
    m_attr_endY = 0.0;
    m_has_attr_centralX = false;
    m_attr_centralX = 0.0;
    m_has_attr_centralY = false;
    m_attr_centralY = 0.0;
    m_has_attr_focalX = false;
    m_attr_focalX = 0.0;
    m_has_attr_focalY = false;
    m_attr_focalY = 0.0;
    m_has_attr_radius = false;
    m_attr_radius = 0.0;
    m_has_attr_angle = false;
    m_attr_angle = 0.0;
    m_has_attr_type = false;
    m_has_attr_spread = false;
    m_has_attr_coordinateMode = false;
}

DomGradient::~DomGradient()
{
    for (int i = 0; i < m_gradientStop.size(); ++i)
        delete m_gradientStop[i];
    m_gradientStop.clear();
}

void DomGradient::read(const QDomElement &node)
{
    if (node.hasAttribute(QLatin1String("startx")))
        setAttributeStartX(node.attribute(QLatin1String("startx")).toDouble());
    if (node.hasAttribute(QLatin1String("starty")))
        setAttributeStartY(node.attribute(QLatin1String("starty")).toDouble());
    if (node.hasAttribute(QLatin1String("endx")))
        setAttributeEndX(node.attribute(QLatin1String("endx")).toDouble());
    if (node.hasAttribute(QLatin1String("endy")))
        setAttributeEndY(node.attribute(QLatin1String("endy")).toDouble());
    if (node.hasAttribute(QLatin1String("centralx")))
        setAttributeCentralX(node.attribute(QLatin1String("centralx")).toDouble());
    if (node.hasAttribute(QLatin1String("centraly")))
        setAttributeCentralY(node.attribute(QLatin1String("centraly")).toDouble());
    if (node.hasAttribute(QLatin1String("focalx")))
        setAttributeFocalX(node.attribute(QLatin1String("focalx")).toDouble());
    if (node.hasAttribute(QLatin1String("focaly")))
        setAttributeFocalY(node.attribute(QLatin1String("focaly")).toDouble());
    if (node.hasAttribute(QLatin1String("radius")))
        setAttributeRadius(node.attribute(QLatin1String("radius")).toDouble());
    if (node.hasAttribute(QLatin1String("angle")))
        setAttributeAngle(node.attribute(QLatin1String("angle")).toDouble());
    if (node.hasAttribute(QLatin1String("type")))
        setAttributeType(node.attribute(QLatin1String("type")));
    if (node.hasAttribute(QLatin1String("spread")))
        setAttributeSpread(node.attribute(QLatin1String("spread")));
    if (node.hasAttribute(QLatin1String("coordinatemode")))
        setAttributeCoordinateMode(node.attribute(QLatin1String("coordinatemode")));

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("gradientstop")) {
            DomGradientStop *v = new DomGradientStop();
            v->read(e);
            m_gradientStop.append(v);
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomGradient::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("gradient") : tagName.toLower());

    QDomElement child;

    if (hasAttributeStartX())
        e.setAttribute(QLatin1String("startx"), attributeStartX());

    if (hasAttributeStartY())
        e.setAttribute(QLatin1String("starty"), attributeStartY());

    if (hasAttributeEndX())
        e.setAttribute(QLatin1String("endx"), attributeEndX());

    if (hasAttributeEndY())
        e.setAttribute(QLatin1String("endy"), attributeEndY());

    if (hasAttributeCentralX())
        e.setAttribute(QLatin1String("centralx"), attributeCentralX());

    if (hasAttributeCentralY())
        e.setAttribute(QLatin1String("centraly"), attributeCentralY());

    if (hasAttributeFocalX())
        e.setAttribute(QLatin1String("focalx"), attributeFocalX());

    if (hasAttributeFocalY())
        e.setAttribute(QLatin1String("focaly"), attributeFocalY());

    if (hasAttributeRadius())
        e.setAttribute(QLatin1String("radius"), attributeRadius());

    if (hasAttributeAngle())
        e.setAttribute(QLatin1String("angle"), attributeAngle());

    if (hasAttributeType())
        e.setAttribute(QLatin1String("type"), attributeType());

    if (hasAttributeSpread())
        e.setAttribute(QLatin1String("spread"), attributeSpread());

    if (hasAttributeCoordinateMode())
        e.setAttribute(QLatin1String("coordinatemode"), attributeCoordinateMode());

    for (int i = 0; i < m_gradientStop.size(); ++i) {
        DomGradientStop* v = m_gradientStop[i];
        QDomNode child = v->write(doc, QLatin1String("gradientstop"));
        e.appendChild(child);
    }
    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomGradient::setElementGradientStop(const QList<DomGradientStop*>& a)
{
    m_gradientStop = a;
}

void DomBrush::clear(bool clear_all)
{
    delete m_color;
    delete m_texture;
    delete m_gradient;

    if (clear_all) {
    m_text = QString();
    m_has_attr_brushStyle = false;
    }

    m_kind = Unknown;

    m_color = 0;
    m_texture = 0;
    m_gradient = 0;
}

DomBrush::DomBrush()
{
    m_kind = Unknown;

    m_has_attr_brushStyle = false;
    m_color = 0;
    m_texture = 0;
    m_gradient = 0;
}

DomBrush::~DomBrush()
{
    delete m_color;
    delete m_texture;
    delete m_gradient;
}

void DomBrush::read(const QDomElement &node)
{
    if (node.hasAttribute(QLatin1String("brushstyle")))
        setAttributeBrushStyle(node.attribute(QLatin1String("brushstyle")));

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("color")) {
            DomColor *v = new DomColor();
            v->read(e);
            setElementColor(v);
            continue;
        }
        if (tag == QLatin1String("texture")) {
            DomProperty *v = new DomProperty();
            v->read(e);
            setElementTexture(v);
            continue;
        }
        if (tag == QLatin1String("gradient")) {
            DomGradient *v = new DomGradient();
            v->read(e);
            setElementGradient(v);
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomBrush::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("brush") : tagName.toLower());

    QDomElement child;

    if (hasAttributeBrushStyle())
        e.setAttribute(QLatin1String("brushstyle"), attributeBrushStyle());

    switch(kind()) {
        case Color: {
            DomColor* v = elementColor();
            if (v != 0) {
                QDomElement child = v->write(doc, QLatin1String("color"));
                e.appendChild(child);
            }
            break;
        }
        case Texture: {
            DomProperty* v = elementTexture();
            if (v != 0) {
                QDomElement child = v->write(doc, QLatin1String("texture"));
                e.appendChild(child);
            }
            break;
        }
        case Gradient: {
            DomGradient* v = elementGradient();
            if (v != 0) {
                QDomElement child = v->write(doc, QLatin1String("gradient"));
                e.appendChild(child);
            }
            break;
        }
        default:
            break;
    }
    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomBrush::setElementColor(DomColor* a)
{
    clear(false);
    m_kind = Color;
    m_color = a;
}

void DomBrush::setElementTexture(DomProperty* a)
{
    clear(false);
    m_kind = Texture;
    m_texture = a;
}

void DomBrush::setElementGradient(DomGradient* a)
{
    clear(false);
    m_kind = Gradient;
    m_gradient = a;
}

void DomColorRole::clear(bool clear_all)
{
    delete m_brush;

    if (clear_all) {
    m_text = QString();
    m_has_attr_role = false;
    }

    m_children = 0;
    m_brush = 0;
}

DomColorRole::DomColorRole()
{
    m_children = 0;
    m_has_attr_role = false;
    m_brush = 0;
}

DomColorRole::~DomColorRole()
{
    delete m_brush;
}

void DomColorRole::read(const QDomElement &node)
{
    if (node.hasAttribute(QLatin1String("role")))
        setAttributeRole(node.attribute(QLatin1String("role")));

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("brush")) {
            DomBrush *v = new DomBrush();
            v->read(e);
            setElementBrush(v);
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomColorRole::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("colorrole") : tagName.toLower());

    QDomElement child;

    if (hasAttributeRole())
        e.setAttribute(QLatin1String("role"), attributeRole());

    if (m_children & Brush) {
        e.appendChild(m_brush->write(doc, QLatin1String("brush")));
    }

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomColorRole::setElementBrush(DomBrush* a)
{
    delete m_brush;
    m_children |= Brush;
    m_brush = a;
}

void DomColorRole::clearElementBrush()
{
    delete m_brush;
    m_brush = 0;
    m_children &= ~Brush;
}

void DomColorGroup::clear(bool clear_all)
{
    for (int i = 0; i < m_colorRole.size(); ++i)
        delete m_colorRole[i];
    m_colorRole.clear();
    for (int i = 0; i < m_color.size(); ++i)
        delete m_color[i];
    m_color.clear();

    if (clear_all) {
    m_text = QString();
    }

}

DomColorGroup::DomColorGroup()
{
}

DomColorGroup::~DomColorGroup()
{
    for (int i = 0; i < m_colorRole.size(); ++i)
        delete m_colorRole[i];
    m_colorRole.clear();
    for (int i = 0; i < m_color.size(); ++i)
        delete m_color[i];
    m_color.clear();
}

void DomColorGroup::read(const QDomElement &node)
{

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("colorrole")) {
            DomColorRole *v = new DomColorRole();
            v->read(e);
            m_colorRole.append(v);
            continue;
        }
        if (tag == QLatin1String("color")) {
            DomColor *v = new DomColor();
            v->read(e);
            m_color.append(v);
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomColorGroup::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("colorgroup") : tagName.toLower());

    QDomElement child;

    for (int i = 0; i < m_colorRole.size(); ++i) {
        DomColorRole* v = m_colorRole[i];
        QDomNode child = v->write(doc, QLatin1String("colorrole"));
        e.appendChild(child);
    }
    for (int i = 0; i < m_color.size(); ++i) {
        DomColor* v = m_color[i];
        QDomNode child = v->write(doc, QLatin1String("color"));
        e.appendChild(child);
    }
    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomColorGroup::setElementColorRole(const QList<DomColorRole*>& a)
{
    m_colorRole = a;
}

void DomColorGroup::setElementColor(const QList<DomColor*>& a)
{
    m_color = a;
}

void DomPalette::clear(bool clear_all)
{
    delete m_active;
    delete m_inactive;
    delete m_disabled;

    if (clear_all) {
    m_text = QString();
    }

    m_children = 0;
    m_active = 0;
    m_inactive = 0;
    m_disabled = 0;
}

DomPalette::DomPalette()
{
    m_children = 0;
    m_active = 0;
    m_inactive = 0;
    m_disabled = 0;
}

DomPalette::~DomPalette()
{
    delete m_active;
    delete m_inactive;
    delete m_disabled;
}

void DomPalette::read(const QDomElement &node)
{

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("active")) {
            DomColorGroup *v = new DomColorGroup();
            v->read(e);
            setElementActive(v);
            continue;
        }
        if (tag == QLatin1String("inactive")) {
            DomColorGroup *v = new DomColorGroup();
            v->read(e);
            setElementInactive(v);
            continue;
        }
        if (tag == QLatin1String("disabled")) {
            DomColorGroup *v = new DomColorGroup();
            v->read(e);
            setElementDisabled(v);
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomPalette::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("palette") : tagName.toLower());

    QDomElement child;

    if (m_children & Active) {
        e.appendChild(m_active->write(doc, QLatin1String("active")));
    }

    if (m_children & Inactive) {
        e.appendChild(m_inactive->write(doc, QLatin1String("inactive")));
    }

    if (m_children & Disabled) {
        e.appendChild(m_disabled->write(doc, QLatin1String("disabled")));
    }

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomPalette::setElementActive(DomColorGroup* a)
{
    delete m_active;
    m_children |= Active;
    m_active = a;
}

void DomPalette::setElementInactive(DomColorGroup* a)
{
    delete m_inactive;
    m_children |= Inactive;
    m_inactive = a;
}

void DomPalette::setElementDisabled(DomColorGroup* a)
{
    delete m_disabled;
    m_children |= Disabled;
    m_disabled = a;
}

void DomPalette::clearElementActive()
{
    delete m_active;
    m_active = 0;
    m_children &= ~Active;
}

void DomPalette::clearElementInactive()
{
    delete m_inactive;
    m_inactive = 0;
    m_children &= ~Inactive;
}

void DomPalette::clearElementDisabled()
{
    delete m_disabled;
    m_disabled = 0;
    m_children &= ~Disabled;
}

void DomFont::clear(bool clear_all)
{

    if (clear_all) {
    m_text = QString();
    }

    m_children = 0;
    m_pointSize = 0;
    m_weight = 0;
    m_italic = false;
    m_bold = false;
    m_underline = false;
    m_strikeOut = false;
    m_antialiasing = false;
    m_kerning = false;
}

DomFont::DomFont()
{
    m_children = 0;
    m_pointSize = 0;
    m_weight = 0;
    m_italic = false;
    m_bold = false;
    m_underline = false;
    m_strikeOut = false;
    m_antialiasing = false;
    m_kerning = false;
}

DomFont::~DomFont()
{
}

void DomFont::read(const QDomElement &node)
{

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("family")) {
            setElementFamily(e.text());
            continue;
        }
        if (tag == QLatin1String("pointsize")) {
            setElementPointSize(e.text().toInt());
            continue;
        }
        if (tag == QLatin1String("weight")) {
            setElementWeight(e.text().toInt());
            continue;
        }
        if (tag == QLatin1String("italic")) {
            setElementItalic((e.text() == QLatin1String("true") ? true : false));
            continue;
        }
        if (tag == QLatin1String("bold")) {
            setElementBold((e.text() == QLatin1String("true") ? true : false));
            continue;
        }
        if (tag == QLatin1String("underline")) {
            setElementUnderline((e.text() == QLatin1String("true") ? true : false));
            continue;
        }
        if (tag == QLatin1String("strikeout")) {
            setElementStrikeOut((e.text() == QLatin1String("true") ? true : false));
            continue;
        }
        if (tag == QLatin1String("antialiasing")) {
            setElementAntialiasing((e.text() == QLatin1String("true") ? true : false));
            continue;
        }
        if (tag == QLatin1String("stylestrategy")) {
            setElementStyleStrategy(e.text());
            continue;
        }
        if (tag == QLatin1String("kerning")) {
            setElementKerning((e.text() == QLatin1String("true") ? true : false));
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomFont::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("font") : tagName.toLower());

    QDomElement child;

    if (m_children & Family) {
        child = doc.createElement(QLatin1String("family"));
        child.appendChild(doc.createTextNode(m_family));
        e.appendChild(child);
    }

    if (m_children & PointSize) {
        child = doc.createElement(QLatin1String("pointsize"));
        child.appendChild(doc.createTextNode(QString::number(m_pointSize)));
        e.appendChild(child);
    }

    if (m_children & Weight) {
        child = doc.createElement(QLatin1String("weight"));
        child.appendChild(doc.createTextNode(QString::number(m_weight)));
        e.appendChild(child);
    }

    if (m_children & Italic) {
        child = doc.createElement(QLatin1String("italic"));
        child.appendChild(doc.createTextNode((m_italic ? QLatin1String("true") : QLatin1String("false"))));
        e.appendChild(child);
    }

    if (m_children & Bold) {
        child = doc.createElement(QLatin1String("bold"));
        child.appendChild(doc.createTextNode((m_bold ? QLatin1String("true") : QLatin1String("false"))));
        e.appendChild(child);
    }

    if (m_children & Underline) {
        child = doc.createElement(QLatin1String("underline"));
        child.appendChild(doc.createTextNode((m_underline ? QLatin1String("true") : QLatin1String("false"))));
        e.appendChild(child);
    }

    if (m_children & StrikeOut) {
        child = doc.createElement(QLatin1String("strikeout"));
        child.appendChild(doc.createTextNode((m_strikeOut ? QLatin1String("true") : QLatin1String("false"))));
        e.appendChild(child);
    }

    if (m_children & Antialiasing) {
        child = doc.createElement(QLatin1String("antialiasing"));
        child.appendChild(doc.createTextNode((m_antialiasing ? QLatin1String("true") : QLatin1String("false"))));
        e.appendChild(child);
    }

    if (m_children & StyleStrategy) {
        child = doc.createElement(QLatin1String("stylestrategy"));
        child.appendChild(doc.createTextNode(m_styleStrategy));
        e.appendChild(child);
    }

    if (m_children & Kerning) {
        child = doc.createElement(QLatin1String("kerning"));
        child.appendChild(doc.createTextNode((m_kerning ? QLatin1String("true") : QLatin1String("false"))));
        e.appendChild(child);
    }

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomFont::setElementFamily(const QString& a)
{
    m_children |= Family;
    m_family = a;
}

void DomFont::setElementPointSize(int a)
{
    m_children |= PointSize;
    m_pointSize = a;
}

void DomFont::setElementWeight(int a)
{
    m_children |= Weight;
    m_weight = a;
}

void DomFont::setElementItalic(bool a)
{
    m_children |= Italic;
    m_italic = a;
}

void DomFont::setElementBold(bool a)
{
    m_children |= Bold;
    m_bold = a;
}

void DomFont::setElementUnderline(bool a)
{
    m_children |= Underline;
    m_underline = a;
}

void DomFont::setElementStrikeOut(bool a)
{
    m_children |= StrikeOut;
    m_strikeOut = a;
}

void DomFont::setElementAntialiasing(bool a)
{
    m_children |= Antialiasing;
    m_antialiasing = a;
}

void DomFont::setElementStyleStrategy(const QString& a)
{
    m_children |= StyleStrategy;
    m_styleStrategy = a;
}

void DomFont::setElementKerning(bool a)
{
    m_children |= Kerning;
    m_kerning = a;
}

void DomFont::clearElementFamily()
{
    m_children &= ~Family;
}

void DomFont::clearElementPointSize()
{
    m_children &= ~PointSize;
}

void DomFont::clearElementWeight()
{
    m_children &= ~Weight;
}

void DomFont::clearElementItalic()
{
    m_children &= ~Italic;
}

void DomFont::clearElementBold()
{
    m_children &= ~Bold;
}

void DomFont::clearElementUnderline()
{
    m_children &= ~Underline;
}

void DomFont::clearElementStrikeOut()
{
    m_children &= ~StrikeOut;
}

void DomFont::clearElementAntialiasing()
{
    m_children &= ~Antialiasing;
}

void DomFont::clearElementStyleStrategy()
{
    m_children &= ~StyleStrategy;
}

void DomFont::clearElementKerning()
{
    m_children &= ~Kerning;
}

void DomPoint::clear(bool clear_all)
{

    if (clear_all) {
    m_text = QString();
    }

    m_children = 0;
    m_x = 0;
    m_y = 0;
}

DomPoint::DomPoint()
{
    m_children = 0;
    m_x = 0;
    m_y = 0;
}

DomPoint::~DomPoint()
{
}

void DomPoint::read(const QDomElement &node)
{

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QString(QLatin1Char('x'))) {
            setElementX(e.text().toInt());
            continue;
        }
        if (tag == QString(QLatin1Char('y'))) {
            setElementY(e.text().toInt());
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomPoint::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("point") : tagName.toLower());

    QDomElement child;

    if (m_children & X) {
        child = doc.createElement(QString(QLatin1Char('x')));
        child.appendChild(doc.createTextNode(QString::number(m_x)));
        e.appendChild(child);
    }

    if (m_children & Y) {
        child = doc.createElement(QString(QLatin1Char('y')));
        child.appendChild(doc.createTextNode(QString::number(m_y)));
        e.appendChild(child);
    }

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomPoint::setElementX(int a)
{
    m_children |= X;
    m_x = a;
}

void DomPoint::setElementY(int a)
{
    m_children |= Y;
    m_y = a;
}

void DomPoint::clearElementX()
{
    m_children &= ~X;
}

void DomPoint::clearElementY()
{
    m_children &= ~Y;
}

void DomRect::clear(bool clear_all)
{

    if (clear_all) {
    m_text = QString();
    }

    m_children = 0;
    m_x = 0;
    m_y = 0;
    m_width = 0;
    m_height = 0;
}

DomRect::DomRect()
{
    m_children = 0;
    m_x = 0;
    m_y = 0;
    m_width = 0;
    m_height = 0;
}

DomRect::~DomRect()
{
}

void DomRect::read(const QDomElement &node)
{

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QString(QLatin1Char('x'))) {
            setElementX(e.text().toInt());
            continue;
        }
        if (tag == QString(QLatin1Char('y'))) {
            setElementY(e.text().toInt());
            continue;
        }
        if (tag == QLatin1String("width")) {
            setElementWidth(e.text().toInt());
            continue;
        }
        if (tag == QLatin1String("height")) {
            setElementHeight(e.text().toInt());
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomRect::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("rect") : tagName.toLower());

    QDomElement child;

    if (m_children & X) {
        child = doc.createElement(QString(QLatin1Char('x')));
        child.appendChild(doc.createTextNode(QString::number(m_x)));
        e.appendChild(child);
    }

    if (m_children & Y) {
        child = doc.createElement(QString(QLatin1Char('y')));
        child.appendChild(doc.createTextNode(QString::number(m_y)));
        e.appendChild(child);
    }

    if (m_children & Width) {
        child = doc.createElement(QLatin1String("width"));
        child.appendChild(doc.createTextNode(QString::number(m_width)));
        e.appendChild(child);
    }

    if (m_children & Height) {
        child = doc.createElement(QLatin1String("height"));
        child.appendChild(doc.createTextNode(QString::number(m_height)));
        e.appendChild(child);
    }

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomRect::setElementX(int a)
{
    m_children |= X;
    m_x = a;
}

void DomRect::setElementY(int a)
{
    m_children |= Y;
    m_y = a;
}

void DomRect::setElementWidth(int a)
{
    m_children |= Width;
    m_width = a;
}

void DomRect::setElementHeight(int a)
{
    m_children |= Height;
    m_height = a;
}

void DomRect::clearElementX()
{
    m_children &= ~X;
}

void DomRect::clearElementY()
{
    m_children &= ~Y;
}

void DomRect::clearElementWidth()
{
    m_children &= ~Width;
}

void DomRect::clearElementHeight()
{
    m_children &= ~Height;
}

void DomLocale::clear(bool clear_all)
{

    if (clear_all) {
    m_text = QString();
    m_has_attr_language = false;
    m_has_attr_country = false;
    }

}

DomLocale::DomLocale()
{
    m_has_attr_language = false;
    m_has_attr_country = false;
}

DomLocale::~DomLocale()
{
}

void DomLocale::read(const QDomElement &node)
{
    if (node.hasAttribute(QLatin1String("language")))
        setAttributeLanguage(node.attribute(QLatin1String("language")));
    if (node.hasAttribute(QLatin1String("country")))
        setAttributeCountry(node.attribute(QLatin1String("country")));

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomLocale::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("locale") : tagName.toLower());

    QDomElement child;

    if (hasAttributeLanguage())
        e.setAttribute(QLatin1String("language"), attributeLanguage());

    if (hasAttributeCountry())
        e.setAttribute(QLatin1String("country"), attributeCountry());

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomSizePolicy::clear(bool clear_all)
{

    if (clear_all) {
    m_text = QString();
    m_has_attr_hSizeType = false;
    m_has_attr_vSizeType = false;
    }

    m_children = 0;
    m_hSizeType = 0;
    m_vSizeType = 0;
    m_horStretch = 0;
    m_verStretch = 0;
}

DomSizePolicy::DomSizePolicy()
{
    m_children = 0;
    m_has_attr_hSizeType = false;
    m_has_attr_vSizeType = false;
    m_hSizeType = 0;
    m_vSizeType = 0;
    m_horStretch = 0;
    m_verStretch = 0;
}

DomSizePolicy::~DomSizePolicy()
{
}

void DomSizePolicy::read(const QDomElement &node)
{
    if (node.hasAttribute(QLatin1String("hsizetype")))
        setAttributeHSizeType(node.attribute(QLatin1String("hsizetype")));
    if (node.hasAttribute(QLatin1String("vsizetype")))
        setAttributeVSizeType(node.attribute(QLatin1String("vsizetype")));

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("hsizetype")) {
            setElementHSizeType(e.text().toInt());
            continue;
        }
        if (tag == QLatin1String("vsizetype")) {
            setElementVSizeType(e.text().toInt());
            continue;
        }
        if (tag == QLatin1String("horstretch")) {
            setElementHorStretch(e.text().toInt());
            continue;
        }
        if (tag == QLatin1String("verstretch")) {
            setElementVerStretch(e.text().toInt());
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomSizePolicy::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("sizepolicy") : tagName.toLower());

    QDomElement child;

    if (hasAttributeHSizeType())
        e.setAttribute(QLatin1String("hsizetype"), attributeHSizeType());

    if (hasAttributeVSizeType())
        e.setAttribute(QLatin1String("vsizetype"), attributeVSizeType());

    if (m_children & HSizeType) {
        child = doc.createElement(QLatin1String("hsizetype"));
        child.appendChild(doc.createTextNode(QString::number(m_hSizeType)));
        e.appendChild(child);
    }

    if (m_children & VSizeType) {
        child = doc.createElement(QLatin1String("vsizetype"));
        child.appendChild(doc.createTextNode(QString::number(m_vSizeType)));
        e.appendChild(child);
    }

    if (m_children & HorStretch) {
        child = doc.createElement(QLatin1String("horstretch"));
        child.appendChild(doc.createTextNode(QString::number(m_horStretch)));
        e.appendChild(child);
    }

    if (m_children & VerStretch) {
        child = doc.createElement(QLatin1String("verstretch"));
        child.appendChild(doc.createTextNode(QString::number(m_verStretch)));
        e.appendChild(child);
    }

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomSizePolicy::setElementHSizeType(int a)
{
    m_children |= HSizeType;
    m_hSizeType = a;
}

void DomSizePolicy::setElementVSizeType(int a)
{
    m_children |= VSizeType;
    m_vSizeType = a;
}

void DomSizePolicy::setElementHorStretch(int a)
{
    m_children |= HorStretch;
    m_horStretch = a;
}

void DomSizePolicy::setElementVerStretch(int a)
{
    m_children |= VerStretch;
    m_verStretch = a;
}

void DomSizePolicy::clearElementHSizeType()
{
    m_children &= ~HSizeType;
}

void DomSizePolicy::clearElementVSizeType()
{
    m_children &= ~VSizeType;
}

void DomSizePolicy::clearElementHorStretch()
{
    m_children &= ~HorStretch;
}

void DomSizePolicy::clearElementVerStretch()
{
    m_children &= ~VerStretch;
}

void DomSize::clear(bool clear_all)
{

    if (clear_all) {
    m_text = QString();
    }

    m_children = 0;
    m_width = 0;
    m_height = 0;
}

DomSize::DomSize()
{
    m_children = 0;
    m_width = 0;
    m_height = 0;
}

DomSize::~DomSize()
{
}

void DomSize::read(const QDomElement &node)
{

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("width")) {
            setElementWidth(e.text().toInt());
            continue;
        }
        if (tag == QLatin1String("height")) {
            setElementHeight(e.text().toInt());
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomSize::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("size") : tagName.toLower());

    QDomElement child;

    if (m_children & Width) {
        child = doc.createElement(QLatin1String("width"));
        child.appendChild(doc.createTextNode(QString::number(m_width)));
        e.appendChild(child);
    }

    if (m_children & Height) {
        child = doc.createElement(QLatin1String("height"));
        child.appendChild(doc.createTextNode(QString::number(m_height)));
        e.appendChild(child);
    }

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomSize::setElementWidth(int a)
{
    m_children |= Width;
    m_width = a;
}

void DomSize::setElementHeight(int a)
{
    m_children |= Height;
    m_height = a;
}

void DomSize::clearElementWidth()
{
    m_children &= ~Width;
}

void DomSize::clearElementHeight()
{
    m_children &= ~Height;
}

void DomDate::clear(bool clear_all)
{

    if (clear_all) {
    m_text = QString();
    }

    m_children = 0;
    m_year = 0;
    m_month = 0;
    m_day = 0;
}

DomDate::DomDate()
{
    m_children = 0;
    m_year = 0;
    m_month = 0;
    m_day = 0;
}

DomDate::~DomDate()
{
}

void DomDate::read(const QDomElement &node)
{

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("year")) {
            setElementYear(e.text().toInt());
            continue;
        }
        if (tag == QLatin1String("month")) {
            setElementMonth(e.text().toInt());
            continue;
        }
        if (tag == QLatin1String("day")) {
            setElementDay(e.text().toInt());
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomDate::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("date") : tagName.toLower());

    QDomElement child;

    if (m_children & Year) {
        child = doc.createElement(QLatin1String("year"));
        child.appendChild(doc.createTextNode(QString::number(m_year)));
        e.appendChild(child);
    }

    if (m_children & Month) {
        child = doc.createElement(QLatin1String("month"));
        child.appendChild(doc.createTextNode(QString::number(m_month)));
        e.appendChild(child);
    }

    if (m_children & Day) {
        child = doc.createElement(QLatin1String("day"));
        child.appendChild(doc.createTextNode(QString::number(m_day)));
        e.appendChild(child);
    }

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomDate::setElementYear(int a)
{
    m_children |= Year;
    m_year = a;
}

void DomDate::setElementMonth(int a)
{
    m_children |= Month;
    m_month = a;
}

void DomDate::setElementDay(int a)
{
    m_children |= Day;
    m_day = a;
}

void DomDate::clearElementYear()
{
    m_children &= ~Year;
}

void DomDate::clearElementMonth()
{
    m_children &= ~Month;
}

void DomDate::clearElementDay()
{
    m_children &= ~Day;
}

void DomTime::clear(bool clear_all)
{

    if (clear_all) {
    m_text = QString();
    }

    m_children = 0;
    m_hour = 0;
    m_minute = 0;
    m_second = 0;
}

DomTime::DomTime()
{
    m_children = 0;
    m_hour = 0;
    m_minute = 0;
    m_second = 0;
}

DomTime::~DomTime()
{
}

void DomTime::read(const QDomElement &node)
{

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("hour")) {
            setElementHour(e.text().toInt());
            continue;
        }
        if (tag == QLatin1String("minute")) {
            setElementMinute(e.text().toInt());
            continue;
        }
        if (tag == QLatin1String("second")) {
            setElementSecond(e.text().toInt());
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomTime::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("time") : tagName.toLower());

    QDomElement child;

    if (m_children & Hour) {
        child = doc.createElement(QLatin1String("hour"));
        child.appendChild(doc.createTextNode(QString::number(m_hour)));
        e.appendChild(child);
    }

    if (m_children & Minute) {
        child = doc.createElement(QLatin1String("minute"));
        child.appendChild(doc.createTextNode(QString::number(m_minute)));
        e.appendChild(child);
    }

    if (m_children & Second) {
        child = doc.createElement(QLatin1String("second"));
        child.appendChild(doc.createTextNode(QString::number(m_second)));
        e.appendChild(child);
    }

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomTime::setElementHour(int a)
{
    m_children |= Hour;
    m_hour = a;
}

void DomTime::setElementMinute(int a)
{
    m_children |= Minute;
    m_minute = a;
}

void DomTime::setElementSecond(int a)
{
    m_children |= Second;
    m_second = a;
}

void DomTime::clearElementHour()
{
    m_children &= ~Hour;
}

void DomTime::clearElementMinute()
{
    m_children &= ~Minute;
}

void DomTime::clearElementSecond()
{
    m_children &= ~Second;
}

void DomDateTime::clear(bool clear_all)
{

    if (clear_all) {
    m_text = QString();
    }

    m_children = 0;
    m_hour = 0;
    m_minute = 0;
    m_second = 0;
    m_year = 0;
    m_month = 0;
    m_day = 0;
}

DomDateTime::DomDateTime()
{
    m_children = 0;
    m_hour = 0;
    m_minute = 0;
    m_second = 0;
    m_year = 0;
    m_month = 0;
    m_day = 0;
}

DomDateTime::~DomDateTime()
{
}

void DomDateTime::read(const QDomElement &node)
{

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("hour")) {
            setElementHour(e.text().toInt());
            continue;
        }
        if (tag == QLatin1String("minute")) {
            setElementMinute(e.text().toInt());
            continue;
        }
        if (tag == QLatin1String("second")) {
            setElementSecond(e.text().toInt());
            continue;
        }
        if (tag == QLatin1String("year")) {
            setElementYear(e.text().toInt());
            continue;
        }
        if (tag == QLatin1String("month")) {
            setElementMonth(e.text().toInt());
            continue;
        }
        if (tag == QLatin1String("day")) {
            setElementDay(e.text().toInt());
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomDateTime::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("datetime") : tagName.toLower());

    QDomElement child;

    if (m_children & Hour) {
        child = doc.createElement(QLatin1String("hour"));
        child.appendChild(doc.createTextNode(QString::number(m_hour)));
        e.appendChild(child);
    }

    if (m_children & Minute) {
        child = doc.createElement(QLatin1String("minute"));
        child.appendChild(doc.createTextNode(QString::number(m_minute)));
        e.appendChild(child);
    }

    if (m_children & Second) {
        child = doc.createElement(QLatin1String("second"));
        child.appendChild(doc.createTextNode(QString::number(m_second)));
        e.appendChild(child);
    }

    if (m_children & Year) {
        child = doc.createElement(QLatin1String("year"));
        child.appendChild(doc.createTextNode(QString::number(m_year)));
        e.appendChild(child);
    }

    if (m_children & Month) {
        child = doc.createElement(QLatin1String("month"));
        child.appendChild(doc.createTextNode(QString::number(m_month)));
        e.appendChild(child);
    }

    if (m_children & Day) {
        child = doc.createElement(QLatin1String("day"));
        child.appendChild(doc.createTextNode(QString::number(m_day)));
        e.appendChild(child);
    }

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomDateTime::setElementHour(int a)
{
    m_children |= Hour;
    m_hour = a;
}

void DomDateTime::setElementMinute(int a)
{
    m_children |= Minute;
    m_minute = a;
}

void DomDateTime::setElementSecond(int a)
{
    m_children |= Second;
    m_second = a;
}

void DomDateTime::setElementYear(int a)
{
    m_children |= Year;
    m_year = a;
}

void DomDateTime::setElementMonth(int a)
{
    m_children |= Month;
    m_month = a;
}

void DomDateTime::setElementDay(int a)
{
    m_children |= Day;
    m_day = a;
}

void DomDateTime::clearElementHour()
{
    m_children &= ~Hour;
}

void DomDateTime::clearElementMinute()
{
    m_children &= ~Minute;
}

void DomDateTime::clearElementSecond()
{
    m_children &= ~Second;
}

void DomDateTime::clearElementYear()
{
    m_children &= ~Year;
}

void DomDateTime::clearElementMonth()
{
    m_children &= ~Month;
}

void DomDateTime::clearElementDay()
{
    m_children &= ~Day;
}

void DomStringList::clear(bool clear_all)
{
    m_string.clear();

    if (clear_all) {
    m_text = QString();
    }

}

DomStringList::DomStringList()
{
}

DomStringList::~DomStringList()
{
    m_string.clear();
}

void DomStringList::read(const QDomElement &node)
{

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("string")) {
            m_string.append(e.text());
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomStringList::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("stringlist") : tagName.toLower());

    QDomElement child;

    for (int i = 0; i < m_string.size(); ++i) {
        QString v = m_string[i];
        QDomNode child = doc.createElement(QLatin1String("string"));
        child.appendChild(doc.createTextNode(v));
        e.appendChild(child);
    }
    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomStringList::setElementString(const QStringList& a)
{
    m_string = a;
}

void DomResourcePixmap::clear(bool clear_all)
{

    if (clear_all) {
    m_text = QString();
    m_has_attr_resource = false;
    m_has_attr_alias = false;
    }

}

DomResourcePixmap::DomResourcePixmap()
{
    m_has_attr_resource = false;
    m_has_attr_alias = false;
}

DomResourcePixmap::~DomResourcePixmap()
{
}

void DomResourcePixmap::read(const QDomElement &node)
{
    if (node.hasAttribute(QLatin1String("resource")))
        setAttributeResource(node.attribute(QLatin1String("resource")));
    if (node.hasAttribute(QLatin1String("alias")))
        setAttributeAlias(node.attribute(QLatin1String("alias")));

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomResourcePixmap::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("resourcepixmap") : tagName.toLower());

    QDomElement child;

    if (hasAttributeResource())
        e.setAttribute(QLatin1String("resource"), attributeResource());

    if (hasAttributeAlias())
        e.setAttribute(QLatin1String("alias"), attributeAlias());

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomResourceIcon::clear(bool clear_all)
{
    delete m_normalOff;
    delete m_normalOn;
    delete m_disabledOff;
    delete m_disabledOn;
    delete m_activeOff;
    delete m_activeOn;
    delete m_selectedOff;
    delete m_selectedOn;

    if (clear_all) {
    m_text = QString();
    m_has_attr_resource = false;
    }

    m_children = 0;
    m_normalOff = 0;
    m_normalOn = 0;
    m_disabledOff = 0;
    m_disabledOn = 0;
    m_activeOff = 0;
    m_activeOn = 0;
    m_selectedOff = 0;
    m_selectedOn = 0;
}

DomResourceIcon::DomResourceIcon()
{
    m_children = 0;
    m_has_attr_resource = false;
    m_normalOff = 0;
    m_normalOn = 0;
    m_disabledOff = 0;
    m_disabledOn = 0;
    m_activeOff = 0;
    m_activeOn = 0;
    m_selectedOff = 0;
    m_selectedOn = 0;
}

DomResourceIcon::~DomResourceIcon()
{
    delete m_normalOff;
    delete m_normalOn;
    delete m_disabledOff;
    delete m_disabledOn;
    delete m_activeOff;
    delete m_activeOn;
    delete m_selectedOff;
    delete m_selectedOn;
}

void DomResourceIcon::read(const QDomElement &node)
{
    if (node.hasAttribute(QLatin1String("resource")))
        setAttributeResource(node.attribute(QLatin1String("resource")));

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("normaloff")) {
            DomResourcePixmap *v = new DomResourcePixmap();
            v->read(e);
            setElementNormalOff(v);
            continue;
        }
        if (tag == QLatin1String("normalon")) {
            DomResourcePixmap *v = new DomResourcePixmap();
            v->read(e);
            setElementNormalOn(v);
            continue;
        }
        if (tag == QLatin1String("disabledoff")) {
            DomResourcePixmap *v = new DomResourcePixmap();
            v->read(e);
            setElementDisabledOff(v);
            continue;
        }
        if (tag == QLatin1String("disabledon")) {
            DomResourcePixmap *v = new DomResourcePixmap();
            v->read(e);
            setElementDisabledOn(v);
            continue;
        }
        if (tag == QLatin1String("activeoff")) {
            DomResourcePixmap *v = new DomResourcePixmap();
            v->read(e);
            setElementActiveOff(v);
            continue;
        }
        if (tag == QLatin1String("activeon")) {
            DomResourcePixmap *v = new DomResourcePixmap();
            v->read(e);
            setElementActiveOn(v);
            continue;
        }
        if (tag == QLatin1String("selectedoff")) {
            DomResourcePixmap *v = new DomResourcePixmap();
            v->read(e);
            setElementSelectedOff(v);
            continue;
        }
        if (tag == QLatin1String("selectedon")) {
            DomResourcePixmap *v = new DomResourcePixmap();
            v->read(e);
            setElementSelectedOn(v);
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomResourceIcon::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("resourceicon") : tagName.toLower());

    QDomElement child;

    if (hasAttributeResource())
        e.setAttribute(QLatin1String("resource"), attributeResource());

    if (m_children & NormalOff) {
        e.appendChild(m_normalOff->write(doc, QLatin1String("normaloff")));
    }

    if (m_children & NormalOn) {
        e.appendChild(m_normalOn->write(doc, QLatin1String("normalon")));
    }

    if (m_children & DisabledOff) {
        e.appendChild(m_disabledOff->write(doc, QLatin1String("disabledoff")));
    }

    if (m_children & DisabledOn) {
        e.appendChild(m_disabledOn->write(doc, QLatin1String("disabledon")));
    }

    if (m_children & ActiveOff) {
        e.appendChild(m_activeOff->write(doc, QLatin1String("activeoff")));
    }

    if (m_children & ActiveOn) {
        e.appendChild(m_activeOn->write(doc, QLatin1String("activeon")));
    }

    if (m_children & SelectedOff) {
        e.appendChild(m_selectedOff->write(doc, QLatin1String("selectedoff")));
    }

    if (m_children & SelectedOn) {
        e.appendChild(m_selectedOn->write(doc, QLatin1String("selectedon")));
    }

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomResourceIcon::setElementNormalOff(DomResourcePixmap* a)
{
    delete m_normalOff;
    m_children |= NormalOff;
    m_normalOff = a;
}

void DomResourceIcon::setElementNormalOn(DomResourcePixmap* a)
{
    delete m_normalOn;
    m_children |= NormalOn;
    m_normalOn = a;
}

void DomResourceIcon::setElementDisabledOff(DomResourcePixmap* a)
{
    delete m_disabledOff;
    m_children |= DisabledOff;
    m_disabledOff = a;
}

void DomResourceIcon::setElementDisabledOn(DomResourcePixmap* a)
{
    delete m_disabledOn;
    m_children |= DisabledOn;
    m_disabledOn = a;
}

void DomResourceIcon::setElementActiveOff(DomResourcePixmap* a)
{
    delete m_activeOff;
    m_children |= ActiveOff;
    m_activeOff = a;
}

void DomResourceIcon::setElementActiveOn(DomResourcePixmap* a)
{
    delete m_activeOn;
    m_children |= ActiveOn;
    m_activeOn = a;
}

void DomResourceIcon::setElementSelectedOff(DomResourcePixmap* a)
{
    delete m_selectedOff;
    m_children |= SelectedOff;
    m_selectedOff = a;
}

void DomResourceIcon::setElementSelectedOn(DomResourcePixmap* a)
{
    delete m_selectedOn;
    m_children |= SelectedOn;
    m_selectedOn = a;
}

void DomResourceIcon::clearElementNormalOff()
{
    delete m_normalOff;
    m_normalOff = 0;
    m_children &= ~NormalOff;
}

void DomResourceIcon::clearElementNormalOn()
{
    delete m_normalOn;
    m_normalOn = 0;
    m_children &= ~NormalOn;
}

void DomResourceIcon::clearElementDisabledOff()
{
    delete m_disabledOff;
    m_disabledOff = 0;
    m_children &= ~DisabledOff;
}

void DomResourceIcon::clearElementDisabledOn()
{
    delete m_disabledOn;
    m_disabledOn = 0;
    m_children &= ~DisabledOn;
}

void DomResourceIcon::clearElementActiveOff()
{
    delete m_activeOff;
    m_activeOff = 0;
    m_children &= ~ActiveOff;
}

void DomResourceIcon::clearElementActiveOn()
{
    delete m_activeOn;
    m_activeOn = 0;
    m_children &= ~ActiveOn;
}

void DomResourceIcon::clearElementSelectedOff()
{
    delete m_selectedOff;
    m_selectedOff = 0;
    m_children &= ~SelectedOff;
}

void DomResourceIcon::clearElementSelectedOn()
{
    delete m_selectedOn;
    m_selectedOn = 0;
    m_children &= ~SelectedOn;
}

void DomString::clear(bool clear_all)
{

    if (clear_all) {
    m_text = QString();
    m_has_attr_notr = false;
    m_has_attr_comment = false;
    }

}

DomString::DomString()
{
    m_has_attr_notr = false;
    m_has_attr_comment = false;
}

DomString::~DomString()
{
}

void DomString::read(const QDomElement &node)
{
    if (node.hasAttribute(QLatin1String("notr")))
        setAttributeNotr(node.attribute(QLatin1String("notr")));
    if (node.hasAttribute(QLatin1String("comment")))
        setAttributeComment(node.attribute(QLatin1String("comment")));

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomString::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("string") : tagName.toLower());

    QDomElement child;

    if (hasAttributeNotr())
        e.setAttribute(QLatin1String("notr"), attributeNotr());

    if (hasAttributeComment())
        e.setAttribute(QLatin1String("comment"), attributeComment());

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomPointF::clear(bool clear_all)
{

    if (clear_all) {
    m_text = QString();
    }

    m_children = 0;
    m_x = 0;
    m_y = 0;
}

DomPointF::DomPointF()
{
    m_children = 0;
    m_x = 0;
    m_y = 0;
}

DomPointF::~DomPointF()
{
}

void DomPointF::read(const QDomElement &node)
{

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QString(QLatin1Char('x'))) {
            setElementX(e.text().toDouble());
            continue;
        }
        if (tag == QString(QLatin1Char('y'))) {
            setElementY(e.text().toDouble());
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomPointF::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("pointf") : tagName.toLower());

    QDomElement child;

    if (m_children & X) {
        child = doc.createElement(QString(QLatin1Char('x')));
        child.appendChild(doc.createTextNode(QString::number(m_x, 'f', 15)));
        e.appendChild(child);
    }

    if (m_children & Y) {
        child = doc.createElement(QString(QLatin1Char('y')));
        child.appendChild(doc.createTextNode(QString::number(m_y, 'f', 15)));
        e.appendChild(child);
    }

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomPointF::setElementX(double a)
{
    m_children |= X;
    m_x = a;
}

void DomPointF::setElementY(double a)
{
    m_children |= Y;
    m_y = a;
}

void DomPointF::clearElementX()
{
    m_children &= ~X;
}

void DomPointF::clearElementY()
{
    m_children &= ~Y;
}

void DomRectF::clear(bool clear_all)
{

    if (clear_all) {
    m_text = QString();
    }

    m_children = 0;
    m_x = 0;
    m_y = 0;
    m_width = 0;
    m_height = 0;
}

DomRectF::DomRectF()
{
    m_children = 0;
    m_x = 0;
    m_y = 0;
    m_width = 0;
    m_height = 0;
}

DomRectF::~DomRectF()
{
}

void DomRectF::read(const QDomElement &node)
{

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QString(QLatin1Char('x'))) {
            setElementX(e.text().toDouble());
            continue;
        }
        if (tag == QString(QLatin1Char('y'))) {
            setElementY(e.text().toDouble());
            continue;
        }
        if (tag == QLatin1String("width")) {
            setElementWidth(e.text().toDouble());
            continue;
        }
        if (tag == QLatin1String("height")) {
            setElementHeight(e.text().toDouble());
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomRectF::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("rectf") : tagName.toLower());

    QDomElement child;

    if (m_children & X) {
        child = doc.createElement(QString(QLatin1Char('x')));
        child.appendChild(doc.createTextNode(QString::number(m_x, 'f', 15)));
        e.appendChild(child);
    }

    if (m_children & Y) {
        child = doc.createElement(QString(QLatin1Char('y')));
        child.appendChild(doc.createTextNode(QString::number(m_y, 'f', 15)));
        e.appendChild(child);
    }

    if (m_children & Width) {
        child = doc.createElement(QLatin1String("width"));
        child.appendChild(doc.createTextNode(QString::number(m_width, 'f', 15)));
        e.appendChild(child);
    }

    if (m_children & Height) {
        child = doc.createElement(QLatin1String("height"));
        child.appendChild(doc.createTextNode(QString::number(m_height, 'f', 15)));
        e.appendChild(child);
    }

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomRectF::setElementX(double a)
{
    m_children |= X;
    m_x = a;
}

void DomRectF::setElementY(double a)
{
    m_children |= Y;
    m_y = a;
}

void DomRectF::setElementWidth(double a)
{
    m_children |= Width;
    m_width = a;
}

void DomRectF::setElementHeight(double a)
{
    m_children |= Height;
    m_height = a;
}

void DomRectF::clearElementX()
{
    m_children &= ~X;
}

void DomRectF::clearElementY()
{
    m_children &= ~Y;
}

void DomRectF::clearElementWidth()
{
    m_children &= ~Width;
}

void DomRectF::clearElementHeight()
{
    m_children &= ~Height;
}

void DomSizeF::clear(bool clear_all)
{

    if (clear_all) {
    m_text = QString();
    }

    m_children = 0;
    m_width = 0;
    m_height = 0;
}

DomSizeF::DomSizeF()
{
    m_children = 0;
    m_width = 0;
    m_height = 0;
}

DomSizeF::~DomSizeF()
{
}

void DomSizeF::read(const QDomElement &node)
{

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("width")) {
            setElementWidth(e.text().toDouble());
            continue;
        }
        if (tag == QLatin1String("height")) {
            setElementHeight(e.text().toDouble());
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomSizeF::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("sizef") : tagName.toLower());

    QDomElement child;

    if (m_children & Width) {
        child = doc.createElement(QLatin1String("width"));
        child.appendChild(doc.createTextNode(QString::number(m_width, 'f', 15)));
        e.appendChild(child);
    }

    if (m_children & Height) {
        child = doc.createElement(QLatin1String("height"));
        child.appendChild(doc.createTextNode(QString::number(m_height, 'f', 15)));
        e.appendChild(child);
    }

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomSizeF::setElementWidth(double a)
{
    m_children |= Width;
    m_width = a;
}

void DomSizeF::setElementHeight(double a)
{
    m_children |= Height;
    m_height = a;
}

void DomSizeF::clearElementWidth()
{
    m_children &= ~Width;
}

void DomSizeF::clearElementHeight()
{
    m_children &= ~Height;
}

void DomChar::clear(bool clear_all)
{

    if (clear_all) {
    m_text = QString();
    }

    m_children = 0;
    m_unicode = 0;
}

DomChar::DomChar()
{
    m_children = 0;
    m_unicode = 0;
}

DomChar::~DomChar()
{
}

void DomChar::read(const QDomElement &node)
{

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("unicode")) {
            setElementUnicode(e.text().toInt());
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomChar::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("char") : tagName.toLower());

    QDomElement child;

    if (m_children & Unicode) {
        child = doc.createElement(QLatin1String("unicode"));
        child.appendChild(doc.createTextNode(QString::number(m_unicode)));
        e.appendChild(child);
    }

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomChar::setElementUnicode(int a)
{
    m_children |= Unicode;
    m_unicode = a;
}

void DomChar::clearElementUnicode()
{
    m_children &= ~Unicode;
}

void DomUrl::clear(bool clear_all)
{
    delete m_string;

    if (clear_all) {
    m_text = QString();
    }

    m_children = 0;
    m_string = 0;
}

DomUrl::DomUrl()
{
    m_children = 0;
    m_string = 0;
}

DomUrl::~DomUrl()
{
    delete m_string;
}

void DomUrl::read(const QDomElement &node)
{

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("string")) {
            DomString *v = new DomString();
            v->read(e);
            setElementString(v);
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomUrl::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("url") : tagName.toLower());

    QDomElement child;

    if (m_children & String) {
        e.appendChild(m_string->write(doc, QLatin1String("string")));
    }

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomUrl::setElementString(DomString* a)
{
    delete m_string;
    m_children |= String;
    m_string = a;
}

void DomUrl::clearElementString()
{
    delete m_string;
    m_string = 0;
    m_children &= ~String;
}

void DomProperty::clear(bool clear_all)
{
    delete m_color;
    delete m_font;
    delete m_iconSet;
    delete m_pixmap;
    delete m_palette;
    delete m_point;
    delete m_rect;
    delete m_locale;
    delete m_sizePolicy;
    delete m_size;
    delete m_string;
    delete m_stringList;
    delete m_date;
    delete m_time;
    delete m_dateTime;
    delete m_pointF;
    delete m_rectF;
    delete m_sizeF;
    delete m_char;
    delete m_url;
    delete m_brush;

    if (clear_all) {
    m_text = QString();
    m_has_attr_name = false;
    m_has_attr_stdset = false;
    m_attr_stdset = 0;
    }

    m_kind = Unknown;

    m_color = 0;
    m_cursor = 0;
    m_font = 0;
    m_iconSet = 0;
    m_pixmap = 0;
    m_palette = 0;
    m_point = 0;
    m_rect = 0;
    m_locale = 0;
    m_sizePolicy = 0;
    m_size = 0;
    m_string = 0;
    m_stringList = 0;
    m_number = 0;
    m_float = 0.0;
    m_double = 0;
    m_date = 0;
    m_time = 0;
    m_dateTime = 0;
    m_pointF = 0;
    m_rectF = 0;
    m_sizeF = 0;
    m_longLong = 0;
    m_char = 0;
    m_url = 0;
    m_uInt = 0;
    m_uLongLong = 0;
    m_brush = 0;
}

DomProperty::DomProperty()
{
    m_kind = Unknown;

    m_has_attr_name = false;
    m_has_attr_stdset = false;
    m_attr_stdset = 0;
    m_color = 0;
    m_cursor = 0;
    m_font = 0;
    m_iconSet = 0;
    m_pixmap = 0;
    m_palette = 0;
    m_point = 0;
    m_rect = 0;
    m_locale = 0;
    m_sizePolicy = 0;
    m_size = 0;
    m_string = 0;
    m_stringList = 0;
    m_number = 0;
    m_float = 0.0;
    m_double = 0;
    m_date = 0;
    m_time = 0;
    m_dateTime = 0;
    m_pointF = 0;
    m_rectF = 0;
    m_sizeF = 0;
    m_longLong = 0;
    m_char = 0;
    m_url = 0;
    m_uInt = 0;
    m_uLongLong = 0;
    m_brush = 0;
}

DomProperty::~DomProperty()
{
    delete m_color;
    delete m_font;
    delete m_iconSet;
    delete m_pixmap;
    delete m_palette;
    delete m_point;
    delete m_rect;
    delete m_locale;
    delete m_sizePolicy;
    delete m_size;
    delete m_string;
    delete m_stringList;
    delete m_date;
    delete m_time;
    delete m_dateTime;
    delete m_pointF;
    delete m_rectF;
    delete m_sizeF;
    delete m_char;
    delete m_url;
    delete m_brush;
}

void DomProperty::read(const QDomElement &node)
{
    if (node.hasAttribute(QLatin1String("name")))
        setAttributeName(node.attribute(QLatin1String("name")));
    if (node.hasAttribute(QLatin1String("stdset")))
        setAttributeStdset(node.attribute(QLatin1String("stdset")).toInt());

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("bool")) {
            setElementBool(e.text());
            continue;
        }
        if (tag == QLatin1String("color")) {
            DomColor *v = new DomColor();
            v->read(e);
            setElementColor(v);
            continue;
        }
        if (tag == QLatin1String("cstring")) {
            setElementCstring(e.text());
            continue;
        }
        if (tag == QLatin1String("cursor")) {
            setElementCursor(e.text().toInt());
            continue;
        }
        if (tag == QLatin1String("cursorshape")) {
            setElementCursorShape(e.text());
            continue;
        }
        if (tag == QLatin1String("enum")) {
            setElementEnum(e.text());
            continue;
        }
        if (tag == QLatin1String("font")) {
            DomFont *v = new DomFont();
            v->read(e);
            setElementFont(v);
            continue;
        }
        if (tag == QLatin1String("iconset")) {
            DomResourceIcon *v = new DomResourceIcon();
            v->read(e);
            setElementIconSet(v);
            continue;
        }
        if (tag == QLatin1String("pixmap")) {
            DomResourcePixmap *v = new DomResourcePixmap();
            v->read(e);
            setElementPixmap(v);
            continue;
        }
        if (tag == QLatin1String("palette")) {
            DomPalette *v = new DomPalette();
            v->read(e);
            setElementPalette(v);
            continue;
        }
        if (tag == QLatin1String("point")) {
            DomPoint *v = new DomPoint();
            v->read(e);
            setElementPoint(v);
            continue;
        }
        if (tag == QLatin1String("rect")) {
            DomRect *v = new DomRect();
            v->read(e);
            setElementRect(v);
            continue;
        }
        if (tag == QLatin1String("set")) {
            setElementSet(e.text());
            continue;
        }
        if (tag == QLatin1String("locale")) {
            DomLocale *v = new DomLocale();
            v->read(e);
            setElementLocale(v);
            continue;
        }
        if (tag == QLatin1String("sizepolicy")) {
            DomSizePolicy *v = new DomSizePolicy();
            v->read(e);
            setElementSizePolicy(v);
            continue;
        }
        if (tag == QLatin1String("size")) {
            DomSize *v = new DomSize();
            v->read(e);
            setElementSize(v);
            continue;
        }
        if (tag == QLatin1String("string")) {
            DomString *v = new DomString();
            v->read(e);
            setElementString(v);
            continue;
        }
        if (tag == QLatin1String("stringlist")) {
            DomStringList *v = new DomStringList();
            v->read(e);
            setElementStringList(v);
            continue;
        }
        if (tag == QLatin1String("number")) {
            setElementNumber(e.text().toInt());
            continue;
        }
        if (tag == QLatin1String("float")) {
            setElementFloat(e.text().toFloat());
            continue;
        }
        if (tag == QLatin1String("double")) {
            setElementDouble(e.text().toDouble());
            continue;
        }
        if (tag == QLatin1String("date")) {
            DomDate *v = new DomDate();
            v->read(e);
            setElementDate(v);
            continue;
        }
        if (tag == QLatin1String("time")) {
            DomTime *v = new DomTime();
            v->read(e);
            setElementTime(v);
            continue;
        }
        if (tag == QLatin1String("datetime")) {
            DomDateTime *v = new DomDateTime();
            v->read(e);
            setElementDateTime(v);
            continue;
        }
        if (tag == QLatin1String("pointf")) {
            DomPointF *v = new DomPointF();
            v->read(e);
            setElementPointF(v);
            continue;
        }
        if (tag == QLatin1String("rectf")) {
            DomRectF *v = new DomRectF();
            v->read(e);
            setElementRectF(v);
            continue;
        }
        if (tag == QLatin1String("sizef")) {
            DomSizeF *v = new DomSizeF();
            v->read(e);
            setElementSizeF(v);
            continue;
        }
        if (tag == QLatin1String("longlong")) {
            setElementLongLong(e.text().toLongLong());
            continue;
        }
        if (tag == QLatin1String("char")) {
            DomChar *v = new DomChar();
            v->read(e);
            setElementChar(v);
            continue;
        }
        if (tag == QLatin1String("url")) {
            DomUrl *v = new DomUrl();
            v->read(e);
            setElementUrl(v);
            continue;
        }
        if (tag == QLatin1String("uint")) {
            setElementUInt(e.text().toUInt());
            continue;
        }
        if (tag == QLatin1String("ulonglong")) {
            setElementULongLong(e.text().toULongLong());
            continue;
        }
        if (tag == QLatin1String("brush")) {
            DomBrush *v = new DomBrush();
            v->read(e);
            setElementBrush(v);
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomProperty::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("property") : tagName.toLower());

    QDomElement child;

    if (hasAttributeName())
        e.setAttribute(QLatin1String("name"), attributeName());

    if (hasAttributeStdset())
        e.setAttribute(QLatin1String("stdset"), attributeStdset());

    switch(kind()) {
        case Bool: {
            QDomElement child = doc.createElement(QLatin1String("bool"));
            QDomText text = doc.createTextNode(elementBool());
            child.appendChild(text);
            e.appendChild(child);
            break;
        }
        case Color: {
            DomColor* v = elementColor();
            if (v != 0) {
                QDomElement child = v->write(doc, QLatin1String("color"));
                e.appendChild(child);
            }
            break;
        }
        case Cstring: {
            QDomElement child = doc.createElement(QLatin1String("cstring"));
            QDomText text = doc.createTextNode(elementCstring());
            child.appendChild(text);
            e.appendChild(child);
            break;
        }
        case Cursor: {
            QDomElement child = doc.createElement(QLatin1String("cursor"));
            QDomText text = doc.createTextNode(QString::number(elementCursor()));
            child.appendChild(text);
            e.appendChild(child);
            break;
        }
        case CursorShape: {
            QDomElement child = doc.createElement(QLatin1String("cursorShape"));
            QDomText text = doc.createTextNode(elementCursorShape());
            child.appendChild(text);
            e.appendChild(child);
            break;
        }
        case Enum: {
            QDomElement child = doc.createElement(QLatin1String("enum"));
            QDomText text = doc.createTextNode(elementEnum());
            child.appendChild(text);
            e.appendChild(child);
            break;
        }
        case Font: {
            DomFont* v = elementFont();
            if (v != 0) {
                QDomElement child = v->write(doc, QLatin1String("font"));
                e.appendChild(child);
            }
            break;
        }
        case IconSet: {
            DomResourceIcon* v = elementIconSet();
            if (v != 0) {
                QDomElement child = v->write(doc, QLatin1String("iconset"));
                e.appendChild(child);
            }
            break;
        }
        case Pixmap: {
            DomResourcePixmap* v = elementPixmap();
            if (v != 0) {
                QDomElement child = v->write(doc, QLatin1String("pixmap"));
                e.appendChild(child);
            }
            break;
        }
        case Palette: {
            DomPalette* v = elementPalette();
            if (v != 0) {
                QDomElement child = v->write(doc, QLatin1String("palette"));
                e.appendChild(child);
            }
            break;
        }
        case Point: {
            DomPoint* v = elementPoint();
            if (v != 0) {
                QDomElement child = v->write(doc, QLatin1String("point"));
                e.appendChild(child);
            }
            break;
        }
        case Rect: {
            DomRect* v = elementRect();
            if (v != 0) {
                QDomElement child = v->write(doc, QLatin1String("rect"));
                e.appendChild(child);
            }
            break;
        }
        case Set: {
            QDomElement child = doc.createElement(QLatin1String("set"));
            QDomText text = doc.createTextNode(elementSet());
            child.appendChild(text);
            e.appendChild(child);
            break;
        }
        case Locale: {
            DomLocale* v = elementLocale();
            if (v != 0) {
                QDomElement child = v->write(doc, QLatin1String("locale"));
                e.appendChild(child);
            }
            break;
        }
        case SizePolicy: {
            DomSizePolicy* v = elementSizePolicy();
            if (v != 0) {
                QDomElement child = v->write(doc, QLatin1String("sizepolicy"));
                e.appendChild(child);
            }
            break;
        }
        case Size: {
            DomSize* v = elementSize();
            if (v != 0) {
                QDomElement child = v->write(doc, QLatin1String("size"));
                e.appendChild(child);
            }
            break;
        }
        case String: {
            DomString* v = elementString();
            if (v != 0) {
                QDomElement child = v->write(doc, QLatin1String("string"));
                e.appendChild(child);
            }
            break;
        }
        case StringList: {
            DomStringList* v = elementStringList();
            if (v != 0) {
                QDomElement child = v->write(doc, QLatin1String("stringlist"));
                e.appendChild(child);
            }
            break;
        }
        case Number: {
            QDomElement child = doc.createElement(QLatin1String("number"));
            QDomText text = doc.createTextNode(QString::number(elementNumber()));
            child.appendChild(text);
            e.appendChild(child);
            break;
        }
        case Float: {
            QDomElement child = doc.createElement(QLatin1String("float"));
            QDomText text = doc.createTextNode(QString::number(elementFloat(), 'f', 8));
            child.appendChild(text);
            e.appendChild(child);
            break;
        }
        case Double: {
            QDomElement child = doc.createElement(QLatin1String("double"));
            QDomText text = doc.createTextNode(QString::number(elementDouble(), 'f', 15));
            child.appendChild(text);
            e.appendChild(child);
            break;
        }
        case Date: {
            DomDate* v = elementDate();
            if (v != 0) {
                QDomElement child = v->write(doc, QLatin1String("date"));
                e.appendChild(child);
            }
            break;
        }
        case Time: {
            DomTime* v = elementTime();
            if (v != 0) {
                QDomElement child = v->write(doc, QLatin1String("time"));
                e.appendChild(child);
            }
            break;
        }
        case DateTime: {
            DomDateTime* v = elementDateTime();
            if (v != 0) {
                QDomElement child = v->write(doc, QLatin1String("datetime"));
                e.appendChild(child);
            }
            break;
        }
        case PointF: {
            DomPointF* v = elementPointF();
            if (v != 0) {
                QDomElement child = v->write(doc, QLatin1String("pointf"));
                e.appendChild(child);
            }
            break;
        }
        case RectF: {
            DomRectF* v = elementRectF();
            if (v != 0) {
                QDomElement child = v->write(doc, QLatin1String("rectf"));
                e.appendChild(child);
            }
            break;
        }
        case SizeF: {
            DomSizeF* v = elementSizeF();
            if (v != 0) {
                QDomElement child = v->write(doc, QLatin1String("sizef"));
                e.appendChild(child);
            }
            break;
        }
        case LongLong: {
            QDomElement child = doc.createElement(QLatin1String("longLong"));
            QDomText text = doc.createTextNode(QString::number(elementLongLong()));
            child.appendChild(text);
            e.appendChild(child);
            break;
        }
        case Char: {
            DomChar* v = elementChar();
            if (v != 0) {
                QDomElement child = v->write(doc, QLatin1String("char"));
                e.appendChild(child);
            }
            break;
        }
        case Url: {
            DomUrl* v = elementUrl();
            if (v != 0) {
                QDomElement child = v->write(doc, QLatin1String("url"));
                e.appendChild(child);
            }
            break;
        }
        case UInt: {
            QDomElement child = doc.createElement(QLatin1String("uInt"));
            QDomText text = doc.createTextNode(QString::number(elementUInt()));
            child.appendChild(text);
            e.appendChild(child);
            break;
        }
        case ULongLong: {
            QDomElement child = doc.createElement(QLatin1String("uLongLong"));
            QDomText text = doc.createTextNode(QString::number(elementULongLong()));
            child.appendChild(text);
            e.appendChild(child);
            break;
        }
        case Brush: {
            DomBrush* v = elementBrush();
            if (v != 0) {
                QDomElement child = v->write(doc, QLatin1String("brush"));
                e.appendChild(child);
            }
            break;
        }
        default:
            break;
    }
    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomProperty::setElementBool(const QString& a)
{
    clear(false);
    m_kind = Bool;
    m_bool = a;
}

void DomProperty::setElementColor(DomColor* a)
{
    clear(false);
    m_kind = Color;
    m_color = a;
}

void DomProperty::setElementCstring(const QString& a)
{
    clear(false);
    m_kind = Cstring;
    m_cstring = a;
}

void DomProperty::setElementCursor(int a)
{
    clear(false);
    m_kind = Cursor;
    m_cursor = a;
}

void DomProperty::setElementCursorShape(const QString& a)
{
    clear(false);
    m_kind = CursorShape;
    m_cursorShape = a;
}

void DomProperty::setElementEnum(const QString& a)
{
    clear(false);
    m_kind = Enum;
    m_enum = a;
}

void DomProperty::setElementFont(DomFont* a)
{
    clear(false);
    m_kind = Font;
    m_font = a;
}

void DomProperty::setElementIconSet(DomResourceIcon* a)
{
    clear(false);
    m_kind = IconSet;
    m_iconSet = a;
}

void DomProperty::setElementPixmap(DomResourcePixmap* a)
{
    clear(false);
    m_kind = Pixmap;
    m_pixmap = a;
}

void DomProperty::setElementPalette(DomPalette* a)
{
    clear(false);
    m_kind = Palette;
    m_palette = a;
}

void DomProperty::setElementPoint(DomPoint* a)
{
    clear(false);
    m_kind = Point;
    m_point = a;
}

void DomProperty::setElementRect(DomRect* a)
{
    clear(false);
    m_kind = Rect;
    m_rect = a;
}

void DomProperty::setElementSet(const QString& a)
{
    clear(false);
    m_kind = Set;
    m_set = a;
}

void DomProperty::setElementLocale(DomLocale* a)
{
    clear(false);
    m_kind = Locale;
    m_locale = a;
}

void DomProperty::setElementSizePolicy(DomSizePolicy* a)
{
    clear(false);
    m_kind = SizePolicy;
    m_sizePolicy = a;
}

void DomProperty::setElementSize(DomSize* a)
{
    clear(false);
    m_kind = Size;
    m_size = a;
}

void DomProperty::setElementString(DomString* a)
{
    clear(false);
    m_kind = String;
    m_string = a;
}

void DomProperty::setElementStringList(DomStringList* a)
{
    clear(false);
    m_kind = StringList;
    m_stringList = a;
}

void DomProperty::setElementNumber(int a)
{
    clear(false);
    m_kind = Number;
    m_number = a;
}

void DomProperty::setElementFloat(float a)
{
    clear(false);
    m_kind = Float;
    m_float = a;
}

void DomProperty::setElementDouble(double a)
{
    clear(false);
    m_kind = Double;
    m_double = a;
}

void DomProperty::setElementDate(DomDate* a)
{
    clear(false);
    m_kind = Date;
    m_date = a;
}

void DomProperty::setElementTime(DomTime* a)
{
    clear(false);
    m_kind = Time;
    m_time = a;
}

void DomProperty::setElementDateTime(DomDateTime* a)
{
    clear(false);
    m_kind = DateTime;
    m_dateTime = a;
}

void DomProperty::setElementPointF(DomPointF* a)
{
    clear(false);
    m_kind = PointF;
    m_pointF = a;
}

void DomProperty::setElementRectF(DomRectF* a)
{
    clear(false);
    m_kind = RectF;
    m_rectF = a;
}

void DomProperty::setElementSizeF(DomSizeF* a)
{
    clear(false);
    m_kind = SizeF;
    m_sizeF = a;
}

void DomProperty::setElementLongLong(qlonglong a)
{
    clear(false);
    m_kind = LongLong;
    m_longLong = a;
}

void DomProperty::setElementChar(DomChar* a)
{
    clear(false);
    m_kind = Char;
    m_char = a;
}

void DomProperty::setElementUrl(DomUrl* a)
{
    clear(false);
    m_kind = Url;
    m_url = a;
}

void DomProperty::setElementUInt(uint a)
{
    clear(false);
    m_kind = UInt;
    m_uInt = a;
}

void DomProperty::setElementULongLong(qulonglong a)
{
    clear(false);
    m_kind = ULongLong;
    m_uLongLong = a;
}

void DomProperty::setElementBrush(DomBrush* a)
{
    clear(false);
    m_kind = Brush;
    m_brush = a;
}

void DomConnections::clear(bool clear_all)
{
    for (int i = 0; i < m_connection.size(); ++i)
        delete m_connection[i];
    m_connection.clear();

    if (clear_all) {
    m_text = QString();
    }

}

DomConnections::DomConnections()
{
}

DomConnections::~DomConnections()
{
    for (int i = 0; i < m_connection.size(); ++i)
        delete m_connection[i];
    m_connection.clear();
}

void DomConnections::read(const QDomElement &node)
{

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("connection")) {
            DomConnection *v = new DomConnection();
            v->read(e);
            m_connection.append(v);
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomConnections::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("connections") : tagName.toLower());

    QDomElement child;

    for (int i = 0; i < m_connection.size(); ++i) {
        DomConnection* v = m_connection[i];
        QDomNode child = v->write(doc, QLatin1String("connection"));
        e.appendChild(child);
    }
    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomConnections::setElementConnection(const QList<DomConnection*>& a)
{
    m_connection = a;
}

void DomConnection::clear(bool clear_all)
{
    delete m_hints;

    if (clear_all) {
    m_text = QString();
    }

    m_children = 0;
    m_hints = 0;
}

DomConnection::DomConnection()
{
    m_children = 0;
    m_hints = 0;
}

DomConnection::~DomConnection()
{
    delete m_hints;
}

void DomConnection::read(const QDomElement &node)
{

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("sender")) {
            setElementSender(e.text());
            continue;
        }
        if (tag == QLatin1String("signal")) {
            setElementSignal(e.text());
            continue;
        }
        if (tag == QLatin1String("receiver")) {
            setElementReceiver(e.text());
            continue;
        }
        if (tag == QLatin1String("slot")) {
            setElementSlot(e.text());
            continue;
        }
        if (tag == QLatin1String("hints")) {
            DomConnectionHints *v = new DomConnectionHints();
            v->read(e);
            setElementHints(v);
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomConnection::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("connection") : tagName.toLower());

    QDomElement child;

    if (m_children & Sender) {
        child = doc.createElement(QLatin1String("sender"));
        child.appendChild(doc.createTextNode(m_sender));
        e.appendChild(child);
    }

    if (m_children & Signal) {
        child = doc.createElement(QLatin1String("signal"));
        child.appendChild(doc.createTextNode(m_signal));
        e.appendChild(child);
    }

    if (m_children & Receiver) {
        child = doc.createElement(QLatin1String("receiver"));
        child.appendChild(doc.createTextNode(m_receiver));
        e.appendChild(child);
    }

    if (m_children & Slot) {
        child = doc.createElement(QLatin1String("slot"));
        child.appendChild(doc.createTextNode(m_slot));
        e.appendChild(child);
    }

    if (m_children & Hints) {
        e.appendChild(m_hints->write(doc, QLatin1String("hints")));
    }

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomConnection::setElementSender(const QString& a)
{
    m_children |= Sender;
    m_sender = a;
}

void DomConnection::setElementSignal(const QString& a)
{
    m_children |= Signal;
    m_signal = a;
}

void DomConnection::setElementReceiver(const QString& a)
{
    m_children |= Receiver;
    m_receiver = a;
}

void DomConnection::setElementSlot(const QString& a)
{
    m_children |= Slot;
    m_slot = a;
}

void DomConnection::setElementHints(DomConnectionHints* a)
{
    delete m_hints;
    m_children |= Hints;
    m_hints = a;
}

void DomConnection::clearElementSender()
{
    m_children &= ~Sender;
}

void DomConnection::clearElementSignal()
{
    m_children &= ~Signal;
}

void DomConnection::clearElementReceiver()
{
    m_children &= ~Receiver;
}

void DomConnection::clearElementSlot()
{
    m_children &= ~Slot;
}

void DomConnection::clearElementHints()
{
    delete m_hints;
    m_hints = 0;
    m_children &= ~Hints;
}

void DomConnectionHints::clear(bool clear_all)
{
    for (int i = 0; i < m_hint.size(); ++i)
        delete m_hint[i];
    m_hint.clear();

    if (clear_all) {
    m_text = QString();
    }

}

DomConnectionHints::DomConnectionHints()
{
}

DomConnectionHints::~DomConnectionHints()
{
    for (int i = 0; i < m_hint.size(); ++i)
        delete m_hint[i];
    m_hint.clear();
}

void DomConnectionHints::read(const QDomElement &node)
{

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("hint")) {
            DomConnectionHint *v = new DomConnectionHint();
            v->read(e);
            m_hint.append(v);
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomConnectionHints::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("connectionhints") : tagName.toLower());

    QDomElement child;

    for (int i = 0; i < m_hint.size(); ++i) {
        DomConnectionHint* v = m_hint[i];
        QDomNode child = v->write(doc, QLatin1String("hint"));
        e.appendChild(child);
    }
    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomConnectionHints::setElementHint(const QList<DomConnectionHint*>& a)
{
    m_hint = a;
}

void DomConnectionHint::clear(bool clear_all)
{

    if (clear_all) {
    m_text = QString();
    m_has_attr_type = false;
    }

    m_children = 0;
    m_x = 0;
    m_y = 0;
}

DomConnectionHint::DomConnectionHint()
{
    m_children = 0;
    m_has_attr_type = false;
    m_x = 0;
    m_y = 0;
}

DomConnectionHint::~DomConnectionHint()
{
}

void DomConnectionHint::read(const QDomElement &node)
{
    if (node.hasAttribute(QLatin1String("type")))
        setAttributeType(node.attribute(QLatin1String("type")));

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QString(QLatin1Char('x'))) {
            setElementX(e.text().toInt());
            continue;
        }
        if (tag == QString(QLatin1Char('y'))) {
            setElementY(e.text().toInt());
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomConnectionHint::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("connectionhint") : tagName.toLower());

    QDomElement child;

    if (hasAttributeType())
        e.setAttribute(QLatin1String("type"), attributeType());

    if (m_children & X) {
        child = doc.createElement(QString(QLatin1Char('x')));
        child.appendChild(doc.createTextNode(QString::number(m_x)));
        e.appendChild(child);
    }

    if (m_children & Y) {
        child = doc.createElement(QString(QLatin1Char('y')));
        child.appendChild(doc.createTextNode(QString::number(m_y)));
        e.appendChild(child);
    }

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomConnectionHint::setElementX(int a)
{
    m_children |= X;
    m_x = a;
}

void DomConnectionHint::setElementY(int a)
{
    m_children |= Y;
    m_y = a;
}

void DomConnectionHint::clearElementX()
{
    m_children &= ~X;
}

void DomConnectionHint::clearElementY()
{
    m_children &= ~Y;
}

void DomScript::clear(bool clear_all)
{

    if (clear_all) {
    m_text = QString();
    m_has_attr_source = false;
    m_has_attr_language = false;
    }

}

DomScript::DomScript()
{
    m_has_attr_source = false;
    m_has_attr_language = false;
}

DomScript::~DomScript()
{
}

void DomScript::read(const QDomElement &node)
{
    if (node.hasAttribute(QLatin1String("source")))
        setAttributeSource(node.attribute(QLatin1String("source")));
    if (node.hasAttribute(QLatin1String("language")))
        setAttributeLanguage(node.attribute(QLatin1String("language")));

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomScript::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("script") : tagName.toLower());

    QDomElement child;

    if (hasAttributeSource())
        e.setAttribute(QLatin1String("source"), attributeSource());

    if (hasAttributeLanguage())
        e.setAttribute(QLatin1String("language"), attributeLanguage());

    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomWidgetData::clear(bool clear_all)
{
    for (int i = 0; i < m_property.size(); ++i)
        delete m_property[i];
    m_property.clear();

    if (clear_all) {
    m_text = QString();
    }

}

DomWidgetData::DomWidgetData()
{
}

DomWidgetData::~DomWidgetData()
{
    for (int i = 0; i < m_property.size(); ++i)
        delete m_property[i];
    m_property.clear();
}

void DomWidgetData::read(const QDomElement &node)
{

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("property")) {
            DomProperty *v = new DomProperty();
            v->read(e);
            m_property.append(v);
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomWidgetData::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("widgetdata") : tagName.toLower());

    QDomElement child;

    for (int i = 0; i < m_property.size(); ++i) {
        DomProperty* v = m_property[i];
        QDomNode child = v->write(doc, QLatin1String("property"));
        e.appendChild(child);
    }
    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomWidgetData::setElementProperty(const QList<DomProperty*>& a)
{
    m_property = a;
}

void DomDesignerData::clear(bool clear_all)
{
    for (int i = 0; i < m_property.size(); ++i)
        delete m_property[i];
    m_property.clear();

    if (clear_all) {
    m_text = QString();
    }

}

DomDesignerData::DomDesignerData()
{
}

DomDesignerData::~DomDesignerData()
{
    for (int i = 0; i < m_property.size(); ++i)
        delete m_property[i];
    m_property.clear();
}

void DomDesignerData::read(const QDomElement &node)
{

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("property")) {
            DomProperty *v = new DomProperty();
            v->read(e);
            m_property.append(v);
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomDesignerData::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("designerdata") : tagName.toLower());

    QDomElement child;

    for (int i = 0; i < m_property.size(); ++i) {
        DomProperty* v = m_property[i];
        QDomNode child = v->write(doc, QLatin1String("property"));
        e.appendChild(child);
    }
    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomDesignerData::setElementProperty(const QList<DomProperty*>& a)
{
    m_property = a;
}

void DomSlots::clear(bool clear_all)
{
    m_slot.clear();
    m_signal.clear();

    if (clear_all) {
    m_text = QString();
    }

}

DomSlots::DomSlots()
{
}

DomSlots::~DomSlots()
{
    m_slot.clear();
    m_signal.clear();
}

void DomSlots::read(const QDomElement &node)
{

    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        QString tag = e.tagName().toLower();
        if (tag == QLatin1String("slot")) {
            m_slot.append(e.text());
            continue;
        }
        if (tag == QLatin1String("signal")) {
            m_signal.append(e.text());
            continue;
        }
    }

    m_text.clear();
    for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isText())
            m_text.append(child.nodeValue());
    }
}

QDomElement DomSlots::write(QDomDocument &doc, const QString &tagName) const
{
    QDomElement e = doc.createElement(tagName.isEmpty() ? QString::fromUtf8("slots") : tagName.toLower());

    QDomElement child;

    for (int i = 0; i < m_slot.size(); ++i) {
        QString v = m_slot[i];
        QDomNode child = doc.createElement(QLatin1String("slot"));
        child.appendChild(doc.createTextNode(v));
        e.appendChild(child);
    }
    for (int i = 0; i < m_signal.size(); ++i) {
        QString v = m_signal[i];
        QDomNode child = doc.createElement(QLatin1String("signal"));
        child.appendChild(doc.createTextNode(v));
        e.appendChild(child);
    }
    if (!m_text.isEmpty())
        e.appendChild(doc.createTextNode(m_text));

    return e;
}

void DomSlots::setElementSlot(const QStringList& a)
{
    m_slot = a;
}

void DomSlots::setElementSignal(const QStringList& a)
{
    m_signal = a;
}

QT_END_NAMESPACE

