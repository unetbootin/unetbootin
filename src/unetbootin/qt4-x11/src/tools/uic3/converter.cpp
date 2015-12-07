/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

#include "ui3reader.h"
#include "parser.h"
#include "domtool.h"
#include "ui4.h"
#include "widgetinfo.h"
#include "globaldefs.h"
#include "qt3to4.h"
#include "utils.h"
#include "option.h"
#include "cppextractimages.h"

#include <QtDebug>
#include <QFile>
#include <QHash>
#include <QPair>
#include <QStringList>
#include <QDateTime>
#include <QRegExp>
#include <QSizePolicy>

#include <stdio.h>
#include <stdlib.h>

QT_BEGIN_NAMESPACE

enum { warnHeaderGeneration = 0 };

#define CONVERT_PROPERTY(o, n) \
    do { \
        if (name == QLatin1String(o) \
                && !WidgetInfo::isValidProperty(className, (o)) \
                && WidgetInfo::isValidProperty(className, (n))) { \
            prop->setAttributeName((n)); \
        } \
    } while (0)

static QString classNameForObjectName(const QDomElement &widget, const QString &objectName)
{
    QList<QDomElement> widgetStack;
    widgetStack.append(widget);
    while (!widgetStack.isEmpty()) {
        QDomElement w = widgetStack.takeFirst();
        QDomElement child = w.firstChild().toElement();
        while (!child.isNull()) {
            if (child.tagName() == QLatin1String("property")
                && child.attribute(QLatin1String("name")) == QLatin1String("name")) {
                QDomElement name = child.firstChild().toElement();
                DomString str;
                str.read(name);
                if (str.text() == objectName)
                    return w.attribute(QLatin1String("class"));
            } else if (child.tagName() == QLatin1String("widget")
                || child.tagName() == QLatin1String("vbox")
                || child.tagName() == QLatin1String("hbox")
                || child.tagName() == QLatin1String("grid")) {
                widgetStack.prepend(child);
            }
            child = child.nextSibling().toElement();
        }
    }
    return QString();
}

// Check for potential KDE classes like
//  K3ListView or KLineEdit as precise as possible
static inline bool isKDEClass(const QString &className)
{
    if (className.indexOf(QLatin1Char(':')) != -1)
        return false;
    const int size = className.size();
    if (size < 3 || className.at(0) != QLatin1Char('K'))
        return false;
    // K3ListView
    if (className.at(1) == QLatin1Char('3')) {
        if (size < 4)
            return false;
        return className.at(2).isUpper() &&  className.at(3).isLower();
    }
    // KLineEdit
    return className.at(1) .isUpper() && className.at(2).isLower();
}

DomUI *Ui3Reader::generateUi4(const QDomElement &widget, bool implicitIncludes)
{
    QDomNodeList nl;
    candidateCustomWidgets.clear();

    QString objClass = getClassName(widget);
    if (objClass.isEmpty())
        return 0;
    QString objName = getObjectName(widget);

    DomUI *ui = new DomUI;
    ui->setAttributeVersion(QLatin1String("4.0"));

    QString pixmapFunction = QLatin1String("qPixmapFromMimeSource");
    QStringList ui_tabstops;
    QStringList ui_custom_slots;
    QList<DomInclude*> ui_includes;
    QList<DomWidget*> ui_toolbars;
    QList<DomWidget*> ui_menubars;
    QList<DomAction*> ui_action_list;
    QList<DomActionGroup*> ui_action_group_list;
    QList<DomCustomWidget*> ui_customwidget_list;
    QList<DomConnection*> ui_connection_list;
    QList<QPair<int, int> > ui_connection_lineinfo_list;
    QString author, comment, exportMacro;
    QString klass;

    for (QDomElement n = root.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement()) {
        QString tagName = n.tagName().toLower();

        if (tagName == QLatin1String("tabstops")) {
            QDomElement n2 = n.firstChild().toElement();
            while (!n2.isNull()) {
                if (n2.tagName().toLower() == QLatin1String("tabstop")) {
                    QString name = n2.firstChild().toText().data();
                    ui_tabstops.append(name);
                }
                n2 = n2.nextSibling().toElement();
            }
        } else if (tagName == QLatin1String("pixmapfunction")) {
            pixmapFunction = n.firstChild().toText().data();
        } else if (tagName == QLatin1String("class")) {
            klass = n.firstChild().toText().data();
        } else if (tagName == QLatin1String("author")) {
            author = n.firstChild().toText().data();
        } else if (tagName == QLatin1String("comment")) {
            comment = n.firstChild().toText().data();
        } else if (tagName == QLatin1String("exportmacro")) {
            exportMacro = n.firstChild().toText().data();
        } else if ( n.tagName() == QLatin1String("includehints") ) {
            QDomElement n2 = n.firstChild().toElement();
            while ( !n2.isNull() ) {
                if ( n2.tagName() == QLatin1String("includehint") ) {
                    QString name = n2.firstChild().toText().data();

                    DomInclude *incl = new DomInclude();
                    incl->setText(fixHeaderName(name));
                    incl->setAttributeLocation(n.attribute(QLatin1String("location"), QLatin1String("local")));
                    ui_includes.append(incl);
                }
                n2 = n2.nextSibling().toElement();
            }
        } else if (tagName == QLatin1String("includes")) {
            QDomElement n2 = n.firstChild().toElement();
            while (!n2.isNull()) {
                if (n2.tagName().toLower() == QLatin1String("include")) {
                    QString name = n2.firstChild().toText().data();
                    if (n2.attribute(QLatin1String("impldecl"), QLatin1String("in implementation")) == QLatin1String("in declaration")) {
                        if (name.right(5) == QLatin1String(".ui.h"))
                            continue;

                        DomInclude *incl = new DomInclude();
                        incl->setText(fixHeaderName(name));
                        incl->setAttributeLocation(n2.attribute(QLatin1String("location"), QLatin1String("global")));
                        ui_includes.append(incl);
                    }
                }
                n2 = n2.nextSibling().toElement();
            }
        } else if (tagName == QLatin1String("include")) {
            QString name = n.firstChild().toText().data();
            if (n.attribute(QLatin1String("impldecl"), QLatin1String("in implementation")) == QLatin1String("in declaration")) {
                if (name.right(5) == QLatin1String(".ui.h"))
                    continue;

                DomInclude *incl = new DomInclude();
                incl->setText(fixHeaderName(name));
                incl->setAttributeLocation(n.attribute(QLatin1String("location"), QLatin1String("global")));
                ui_includes.append(incl);
            }
        } else if (tagName == QLatin1String("layoutdefaults")) {
            QString margin = n.attribute(QLatin1String("margin"));
            QString spacing = n.attribute(QLatin1String("spacing"));

            DomLayoutDefault *layoutDefault = new DomLayoutDefault();

            if (!margin.isEmpty())
                layoutDefault->setAttributeMargin(margin.toInt());

            if (!spacing.isEmpty())
                layoutDefault->setAttributeSpacing(spacing.toInt());

            ui->setElementLayoutDefault(layoutDefault);
        } else if (tagName == QLatin1String("layoutfunctions")) {
            QString margin = n.attribute(QLatin1String("margin"));
            QString spacing = n.attribute(QLatin1String("spacing"));

            DomLayoutFunction *layoutDefault = new DomLayoutFunction();

            if (!margin.isEmpty())
                layoutDefault->setAttributeMargin(margin);

            if (!spacing.isEmpty())
                layoutDefault->setAttributeSpacing(spacing);

            ui->setElementLayoutFunction(layoutDefault);
        } else if (tagName == QLatin1String("images")) {
            QDomNodeList nl = n.elementsByTagName(QLatin1String("image"));
            QList<DomImage*> ui_image_list;
            for (int i=0; i<(int)nl.length(); i++) {
                QDomElement e = nl.item(i).toElement();

                QDomElement tmp = e.firstChild().toElement();
                if (tmp.tagName().toLower() != QLatin1String("data"))
                    continue;

                // create the image
                DomImage *img = new DomImage();
                img->setAttributeName(e.attribute(QLatin1String("name")));

                // create the data
                DomImageData *data = new DomImageData();
                img->setElementData(data);

                if (tmp.hasAttribute(QLatin1String("format")))
                    data->setAttributeFormat(tmp.attribute(QLatin1String("format"), QLatin1String("PNG")));

                if (tmp.hasAttribute(QLatin1String("length")))
                    data->setAttributeLength(tmp.attribute(QLatin1String("length")).toInt());

                data->setText(tmp.firstChild().toText().data());

                ui_image_list.append(img);
                QString format = img->elementData()->attributeFormat();
                QString extension = format.left(format.indexOf('.')).toLower();
                m_imageMap[img->attributeName()] = img->attributeName() + QLatin1Char('.') + extension;
            }

            if (ui_image_list.size()) {
                DomImages *images = new DomImages();
                images->setElementImage(ui_image_list);
                ui->setElementImages(images);
            }
        } else if (tagName == QLatin1String("actions")) {
            QDomElement n2 = n.firstChild().toElement();
            while (!n2.isNull()) {
                QString tag = n2.tagName().toLower();

                if (tag == QLatin1String("action")) {
                    DomAction *action = new DomAction();
                    action->read(n2);

                    QList<DomProperty*> properties = action->elementProperty();
                    QString actionName = fixActionProperties(properties);
                    action->setAttributeName(actionName);
                    action->setElementProperty(properties);

                    if (actionName.isEmpty()) {
                        delete action;
                    } else
                        ui_action_list.append(action);
                } else if (tag == QLatin1String("actiongroup")) {
                    DomActionGroup *g= new DomActionGroup();
                    g->read(n2);

                    fixActionGroup(g);
                    ui_action_group_list.append(g);
                }
                n2 = n2.nextSibling().toElement();
            }
        } else if (tagName == QLatin1String("toolbars")) {
            QDomElement n2 = n.firstChild().toElement();
            while (!n2.isNull()) {
                if (n2.tagName().toLower() == QLatin1String("toolbar")) {
                    DomWidget *tb = createWidget(n2, QLatin1String("QToolBar"));
                    ui_toolbars.append(tb);
                }
                n2 = n2.nextSibling().toElement();
            }
        } else if (tagName == QLatin1String("menubar")) {
            DomWidget *tb = createWidget(n, QLatin1String("QMenuBar"));
            ui_menubars.append(tb);
        } else if (tagName == QLatin1String("customwidgets")) {
            QDomElement n2 = n.firstChild().toElement();
            while (!n2.isNull()) {
                if (n2.tagName().toLower() == QLatin1String("customwidget")) {

                    DomCustomWidget *customWidget = new DomCustomWidget;
                    customWidget->read(n2);

                    if (!customWidget->hasElementExtends())
                        customWidget->setElementExtends(QLatin1String("QWidget"));

                    QDomElement n3 = n2.firstChild().toElement();
                    QString cl;

                    QList<DomPropertyData*> ui_property_list;

                    while (!n3.isNull()) {
                        QString tagName = n3.tagName().toLower();

                        if (tagName == QLatin1String("property")) {
                            DomPropertyData *p = new DomPropertyData();
                            p->read(n3);

                            ui_property_list.append(p);
                        }

                        n3 = n3.nextSibling().toElement();
                    }

                    if (ui_property_list.size()) {
                        DomProperties *properties = new DomProperties();
                        properties->setElementProperty(ui_property_list);
                        customWidget->setElementProperties(properties);
                    }

                    ui_customwidget_list.append(customWidget);
                }
                n2 = n2.nextSibling().toElement();
            }
        } else if (tagName == QLatin1String("connections")) {
            QDomElement n2 = n.firstChild().toElement();
            while (!n2.isNull()) {
                if (n2.tagName().toLower() == QLatin1String("connection")) {

                    DomConnection *connection = new DomConnection;
                    connection->read(n2);

                    QString signal = fixMethod(connection->elementSignal());
                    QString slot = fixMethod(connection->elementSlot());
                    connection->setElementSignal(signal);
                    connection->setElementSlot(slot);

                    ui_connection_list.append(connection);
                    ui_connection_lineinfo_list.append(
                        QPair<int, int>(n2.lineNumber(), n2.columnNumber()));
                }
                n2 = n2.nextSibling().toElement();
            }
        } else if (tagName == QLatin1String("slots")) {
            QDomElement n2 = n.firstChild().toElement();
            while (!n2.isNull()) {
                if (n2.tagName().toLower() == QLatin1String("slot")) {
                    QString name = n2.firstChild().toText().data();
                    ui_custom_slots.append(fixMethod(Parser::cleanArgs(name)));
                }
                n2 = n2.nextSibling().toElement();
            }
        }
    }

    // validate the connections
    for (int i = 0; i < ui_connection_list.size(); ++i) {
        DomConnection *conn = ui_connection_list.at(i);
        QPair<int, int> lineinfo = ui_connection_lineinfo_list.at(i);
        QString sender = conn->elementSender();
        QString senderClass = fixClassName(classNameForObjectName(widget, sender));
        QString signal = conn->elementSignal();
        QString receiver = conn->elementReceiver();
        QString receiverClass = fixClassName(classNameForObjectName(widget, receiver));
        QString slot = conn->elementSlot();

        if (!WidgetInfo::isValidSignal(senderClass, signal)) {
            errorInvalidSignal(signal, sender, senderClass,
                               lineinfo.first, lineinfo.second);
        } else if (!WidgetInfo::isValidSlot(receiverClass, slot)) {
            bool resolved = false;
            if (objName == receiver) {
                // see if it's a custom slot
                foreach (QString cs, ui_custom_slots) {
                    if (cs == slot) {
                        resolved = true;
                        break;
                    }
                }
            }
            if (!resolved) {
                errorInvalidSlot(slot, receiver, receiverClass,
                                 lineinfo.first, lineinfo.second);
            }
        }
    }

    DomWidget *w = createWidget(widget);
    Q_ASSERT(w != 0);

    QList<DomWidget*> l = w->elementWidget();
    l += ui_toolbars;
    l += ui_menubars;
    w->setElementWidget(l);

    if (ui_action_group_list.size())
        w->setElementActionGroup(ui_action_group_list);

    if (ui_action_list.size())
        w->setElementAction(ui_action_list);

    ui->setElementWidget(w);

    if (klass.isEmpty())
        klass = w->attributeName();

    ui->setElementClass(klass);
    ui->setElementAuthor(author);
    ui->setElementComment(comment);
    ui->setElementExportMacro(exportMacro);

    if (!ui->elementImages())
        ui->setElementPixmapFunction(pixmapFunction);

    for (int i=0; i<ui_customwidget_list.size(); ++i) {
        const QString name = ui_customwidget_list.at(i)->elementClass();
        if (candidateCustomWidgets.contains(name))
            candidateCustomWidgets.remove(name);
    }


    QMapIterator<QString, bool> it(candidateCustomWidgets);
    while (it.hasNext()) {
        it.next();

        const QString customClass = it.key();
        QString baseClass;

        if (customClass.endsWith(QLatin1String("ListView")))
            baseClass = QLatin1String("Q3ListView");
        else if (customClass.endsWith(QLatin1String("ListBox")))
            baseClass = QLatin1String("Q3ListBox");
        else if (customClass.endsWith(QLatin1String("IconView")))
            baseClass = QLatin1String("Q3IconView");
        else if (customClass.endsWith(QLatin1String("ComboBox")))
            baseClass = QLatin1String("QComboBox");

        if (baseClass.isEmpty())
            continue;

        DomCustomWidget *customWidget = new DomCustomWidget();
        customWidget->setElementClass(customClass);
        customWidget->setElementExtends(baseClass);

        // Magic header generation feature for legacy KDE forms
        // (for example, filesharing/advanced/kcm_sambaconf/share.ui)
        if (implicitIncludes && isKDEClass(customClass)) {
            QString header = customClass.toLower();
            header += QLatin1String(".h");
            DomHeader *domHeader = new DomHeader;
            domHeader->setText(header);
            domHeader->setAttributeLocation(QLatin1String("global"));
            customWidget->setElementHeader(domHeader);
            if (warnHeaderGeneration) {
                const QString msg = QString::fromUtf8("Warning: generated header '%1' for class '%2'.").arg(header).arg(customClass);
                qWarning("%s", qPrintable(msg));
            }
        }
        ui_customwidget_list.append(customWidget);
    }

    if (ui_customwidget_list.size()) {
        DomCustomWidgets *customWidgets = new DomCustomWidgets();
        customWidgets->setElementCustomWidget(ui_customwidget_list);
        ui->setElementCustomWidgets(customWidgets);
    }

    if (ui_tabstops.size()) {
        DomTabStops *tabStops = new DomTabStops();
        tabStops->setElementTabStop(ui_tabstops);
        ui->setElementTabStops(tabStops);
    }

    if (ui_includes.size()) {
        DomIncludes *includes = new DomIncludes();
        includes->setElementInclude(ui_includes);
        ui->setElementIncludes(includes);
    }

    if (ui_connection_list.size()) {
        DomConnections *connections = new DomConnections();
        connections->setElementConnection(ui_connection_list);
        ui->setElementConnections(connections);
    }

    ui->setAttributeStdSetDef(stdsetdef);

    if (m_extractImages) {
        Option opt;
        opt.extractImages = m_extractImages;
        opt.qrcOutputFile = m_qrcOutputFile;
        CPP::ExtractImages(opt).acceptUI(ui);

        ui->clearElementImages();

        DomResources *res = ui->elementResources();
        if (!res) {
            res = new DomResources();
        }
        DomResource *incl = new DomResource();
        incl->setAttributeLocation(m_qrcOutputFile);
        QList<DomResource *> inclList = res->elementInclude();
        inclList.append(incl);
        res->setElementInclude(inclList);
        if (!ui->elementResources())
            ui->setElementResources(res);
    }

    return ui;
}



QString Ui3Reader::fixActionProperties(QList<DomProperty*> &properties,
                                       bool isActionGroup)
{
    QString objectName;

    QMutableListIterator<DomProperty*> it(properties);
    while (it.hasNext()) {
        DomProperty *prop = it.next();
        QString name = prop->attributeName();

        if (name == QLatin1String("name")) {
            objectName = prop->elementCstring();
        } else if (isActionGroup && name == QLatin1String("exclusive")) {
            // continue
        } else if (isActionGroup) {
            errorInvalidProperty(name, objectName, isActionGroup ? QLatin1String("QActionGroup") : QLatin1String("QAction"), -1, -1);
            delete prop;
            it.remove();
        } else if (name == QLatin1String("menuText")) {
            prop->setAttributeName(QLatin1String("text"));
        } else if (name == QLatin1String("text")) {
            prop->setAttributeName(QLatin1String("iconText"));
        } else if (name == QLatin1String("iconSet")) {
            prop->setAttributeName(QLatin1String("icon"));
        } else if (name == QLatin1String("accel")) {
            prop->setAttributeName(QLatin1String("shortcut"));
        } else if (name == QLatin1String("toggleAction")) {
            prop->setAttributeName(QLatin1String("checkable"));
        } else if (name == QLatin1String("on")) {
            prop->setAttributeName(QLatin1String("checked"));
        } else if (!WidgetInfo::isValidProperty(QLatin1String("QAction"), name)) {
            errorInvalidProperty(name, objectName, isActionGroup ? QLatin1String("QActionGroup") : QLatin1String("QAction"), -1, -1);
            delete prop;
            it.remove();
        }
    }

    return objectName;
}

void Ui3Reader::fixActionGroup(DomActionGroup *g)
{
    QList<DomActionGroup*> groups = g->elementActionGroup();
    for (int i=0; i<groups.size(); ++i) {
        fixActionGroup(groups.at(i));
    }

    QList<DomAction*> actions = g->elementAction();
    for (int i=0; i<actions.size(); ++i) {
        DomAction *a = actions.at(i);

        QList<DomProperty*> properties = a->elementProperty();
        QString name = fixActionProperties(properties);
        a->setElementProperty(properties);

        if (name.size())
            a->setAttributeName(name);
    }

    QList<DomProperty*> properties = g->elementProperty();
    QString name = fixActionProperties(properties, true);
    g->setElementProperty(properties);

    if (name.size())
        g->setAttributeName(name);
}

QString Ui3Reader::fixClassName(const QString &className) const
{
    return m_porting->renameClass(className);
}

QString Ui3Reader::fixHeaderName(const QString &headerName) const
{
    return m_porting->renameHeader(headerName);
}

DomWidget *Ui3Reader::createWidget(const QDomElement &w, const QString &widgetClass)
{
    DomWidget *ui_widget = new DomWidget;

    QString className = widgetClass;
    if (className.isEmpty())
        className = w.attribute(QLatin1String("class"));
    className = fixClassName(className);

    if ((className.endsWith(QLatin1String("ListView")) && className != QLatin1String("Q3ListView"))
            || (className.endsWith(QLatin1String("ListBox")) && className != QLatin1String("Q3ListBox"))
            || (className.endsWith(QLatin1String("ComboBox")) && className != QLatin1String("QComboBox"))
            || (className.endsWith(QLatin1String("IconView")) && className != QLatin1String("Q3IconView")))
        candidateCustomWidgets.insert(className, true);

    bool isMenu = (className == QLatin1String("QMenuBar") || className == QLatin1String("QMenu"));

    ui_widget->setAttributeClass(className);

    QList<DomWidget*> ui_child_list;
    QList<DomRow*> ui_row_list;
    QList<DomColumn*> ui_column_list;
    QList<DomItem*> ui_item_list;
    QList<DomProperty*> ui_property_list;
    QList<DomProperty*> ui_attribute_list;
    QList<DomLayout*> ui_layout_list;
    QList<DomActionRef*> ui_action_list;
    QList<DomWidget*> ui_mainwindow_child_list;

    createProperties(w, &ui_property_list, className);
    createAttributes(w, &ui_attribute_list, className);

    DomWidget *ui_mainWindow = 0;
    DomWidget *ui_centralWidget = 0;
    if (className == QLatin1String("QMainWindow") || className == QLatin1String("Q3MainWindow")) {
        ui_centralWidget = new DomWidget;
        ui_centralWidget->setAttributeClass(QLatin1String("QWidget"));
        ui_mainwindow_child_list.append(ui_centralWidget);
        ui_mainWindow = ui_widget;
    }

    QDomElement e = w.firstChild().toElement();
    const bool inQ3ToolBar = className == QLatin1String("Q3ToolBar");
    while (!e.isNull()) {
        QString t = e.tagName().toLower();
        if (t == QLatin1String("vbox") || t == QLatin1String("hbox") || t == QLatin1String("grid")) {
            DomLayout *lay = createLayout(e);
            Q_ASSERT(lay != 0);

            if (ui_layout_list.isEmpty()) {
                ui_layout_list.append(lay);
            } else {
                // it's not possible to have more than one layout for widget!
                delete lay;
            }
        } else if (t == QLatin1String("spacer")) {
            // hmm, spacer as child of a widget.. it doesn't make sense, so skip it!
        } else if (t == QLatin1String("widget")) {
            DomWidget *ui_child = createWidget(e);
            Q_ASSERT(ui_child != 0);

            bool isLayoutWidget = ui_child->attributeClass() == QLatin1String("QLayoutWidget");
            if (isLayoutWidget)
                ui_child->setAttributeClass(QLatin1String("QWidget"));

            foreach (DomLayout *layout, ui_child->elementLayout()) {
                fixLayoutMargin(layout);
            }

            QString widgetClass = ui_child->attributeClass();
            if (widgetClass == QLatin1String("QMenuBar") || widgetClass == QLatin1String("QToolBar")
                    || widgetClass == QLatin1String("QStatusBar")) {
                ui_mainwindow_child_list.append(ui_child);
            } else {
                ui_child_list.append(ui_child);
            }

            if (inQ3ToolBar) {
                DomActionRef *ui_action_ref = new DomActionRef();
                ui_action_ref->setAttributeName(ui_child->attributeName());
                ui_action_list.append(ui_action_ref);
            }
        } else if (t == QLatin1String("action")) {
            DomActionRef *a = new DomActionRef();
            a->read(e);
            ui_action_list.append(a);
        } else if (t == QLatin1String("separator")) {
            DomActionRef *a = new DomActionRef();
            a->setAttributeName(QLatin1String("separator"));
            ui_action_list.append(a);
        } else if (t == QLatin1String("property")) {
            // skip the property it is already handled by createProperties

            QString name = e.attribute(QLatin1String("name"));  // change the varname this widget
            if (name == QLatin1String("name"))
                ui_widget->setAttributeName(DomTool::readProperty(w, QLatin1String("name"), QVariant()).toString());
        } else if (t == QLatin1String("row")) {
            DomRow *row = new DomRow();
            row->read(e);
            ui_row_list.append(row);
        } else if (t == QLatin1String("column")) {
            DomColumn *column = new DomColumn();
            column->read(e);
            ui_column_list.append(column);
        } else if (isMenu && t == QLatin1String("item")) {
            QString text = e.attribute(QLatin1String("text"));
            QString name = e.attribute(QLatin1String("name"));
            QString accel = e.attribute(QLatin1String("accel"));

            QList<DomProperty*> properties;

            DomProperty *atitle = new DomProperty();
            atitle->setAttributeName(QLatin1String("title"));
            DomString *str = new DomString();
            str->setText(text);
            atitle->setElementString(str);
            properties.append(atitle);

            DomWidget *menu = createWidget(e, QLatin1String("QMenu"));
            menu->setAttributeName(name);
            menu->setElementProperty(properties);
            ui_child_list.append(menu);

            DomActionRef *a = new DomActionRef();
            a->setAttributeName(name);
            ui_action_list.append(a);

        } else if (t == QLatin1String("item")) {
            DomItem *item = new DomItem();
            item->read(e);
            ui_item_list.append(item);
        }

        e = e.nextSibling().toElement();
    }

    ui_widget->setElementProperty(ui_property_list);
    ui_widget->setElementAttribute(ui_attribute_list);

    if (ui_centralWidget != 0) {
        Q_ASSERT(ui_mainWindow != 0);
        ui_mainWindow->setElementWidget(ui_mainwindow_child_list);
        ui_widget = ui_centralWidget;
    }

    ui_widget->setElementWidget(ui_child_list);
    ui_widget->setElementAddAction(ui_action_list);
    ui_widget->setElementRow(ui_row_list);
    ui_widget->setElementColumn(ui_column_list);
    ui_widget->setElementItem(ui_item_list);
    ui_widget->setElementLayout(ui_layout_list);

    //ui_widget->setAttributeName(p->elementCstring());

    return ui_mainWindow ? ui_mainWindow : ui_widget;
}

DomLayout *Ui3Reader::createLayout(const QDomElement &w)
{
    DomLayout *lay = new DomLayout();

    QList<DomLayoutItem*> ui_item_list;
    QList<DomProperty*> ui_property_list;
    QList<DomProperty*> ui_attribute_list;

    QString tagName = w.tagName().toLower();

    QString className;
    if (tagName == QLatin1String("vbox"))
        className = QLatin1String("QVBoxLayout");
    else if (tagName == QLatin1String("hbox"))
        className = QLatin1String("QHBoxLayout");
    else
        className = QLatin1String("QGridLayout");

    lay->setAttributeClass(className);

    createProperties(w, &ui_property_list, className);
    createAttributes(w, &ui_attribute_list, className);

    QDomElement e = w.firstChild().toElement();
    while (!e.isNull()) {
        QString t = e.tagName().toLower();
        if (t == QLatin1String("vbox")
                 || t == QLatin1String("hbox")
                 || t == QLatin1String("grid")
                 || t == QLatin1String("spacer")
                 || t == QLatin1String("widget")) {
            DomLayoutItem *lay_item = createLayoutItem(e);
            Q_ASSERT(lay_item != 0);
            ui_item_list.append(lay_item);
        }

        e = e.nextSibling().toElement();
    }

    lay->setElementItem(ui_item_list);
    lay->setElementProperty(ui_property_list);
    lay->setElementAttribute(ui_attribute_list);

    return lay;
}

DomLayoutItem *Ui3Reader::createLayoutItem(const QDomElement &e)
{
    DomLayoutItem *lay_item = new DomLayoutItem;

    QString tagName = e.tagName().toLower();
    if (tagName == QLatin1String("widget")) {
        DomWidget *ui_widget = createWidget(e);
        Q_ASSERT(ui_widget != 0);

        if (ui_widget->attributeClass() == QLatin1String("QLayoutWidget")
                    && ui_widget->elementLayout().size() == 1) {
            QList<DomLayout*> layouts = ui_widget->elementLayout();

            ui_widget->setElementLayout(QList<DomLayout*>());
            delete ui_widget;

            DomLayout *layout = layouts.first();
            fixLayoutMargin(layout);
            lay_item->setElementLayout(layout);
        } else {
            if (ui_widget->attributeClass() == QLatin1String("QLayoutWidget"))
                ui_widget->setAttributeClass(QLatin1String("QWidget"));

            lay_item->setElementWidget(ui_widget);
        }
    } else if (tagName == QLatin1String("spacer")) {
        DomSpacer *ui_spacer = new DomSpacer();
        QList<DomProperty*> properties;

        QByteArray name = DomTool::readProperty(e, QLatin1String("name"), QLatin1String("spacer")).toByteArray();

        Variant var;
        var.createSize(0, 0);

        QVariant def = qVariantFromValue(var);

        Size size = asVariant(DomTool::readProperty(e, QLatin1String("sizeHint"), def)).size;
        QString sizeType = QLatin1String("QSizePolicy::") + DomTool::readProperty(e, QLatin1String("sizeType"), QLatin1String("Expanding")).toString();
        QString orientation = QLatin1String("Qt::") + DomTool::readProperty(e, QLatin1String("orientation"), QLatin1String("Horizontal")).toString();

        ui_spacer->setAttributeName(QLatin1String(name));

        DomProperty *prop = 0;

        // sizeHint
        prop = new DomProperty();
        prop->setAttributeName(QLatin1String("sizeHint"));
        prop->setElementSize(new DomSize());
        prop->elementSize()->setElementWidth(size.width);
        prop->elementSize()->setElementHeight(size.height);
        properties.append(prop);

        // sizeType
        prop = new DomProperty();
        prop->setAttributeName(QLatin1String("sizeType"));
        prop->setElementEnum(sizeType);
        properties.append(prop);

        // orientation
        prop = new DomProperty();
        prop->setAttributeName(QLatin1String("orientation"));
        prop->setElementEnum(orientation);
        properties.append(prop);

        ui_spacer->setElementProperty(properties);
        lay_item->setElementSpacer(ui_spacer);
    } else {
        DomLayout *ui_layout = createLayout(e);
        Q_ASSERT(ui_layout != 0);

        fixLayoutMargin(ui_layout);
        lay_item->setElementLayout(ui_layout);
    }

    if (e.hasAttribute(QLatin1String("row")))
        lay_item->setAttributeRow(e.attribute(QLatin1String("row")).toInt());
    if (e.hasAttribute(QLatin1String("column")))
        lay_item->setAttributeColumn(e.attribute(QLatin1String("column")).toInt());
    if (e.hasAttribute(QLatin1String("rowspan")))
        lay_item->setAttributeRowSpan(e.attribute(QLatin1String("rowspan")).toInt());
    if (e.hasAttribute(QLatin1String("colspan")))
        lay_item->setAttributeColSpan(e.attribute(QLatin1String("colspan")).toInt());

    return lay_item;
}

void Ui3Reader::fixLayoutMargin(DomLayout *ui_layout)
{
    Q_UNUSED(ui_layout)
}

static void addBooleanFontSubProperty(QDomDocument &doc,
                                      const QString &name, const QString &value,
                                      QDomElement &fontElement)
{
    if (value == QLatin1String("true") || value == QLatin1String("1")) {
        QDomElement child = doc.createElement(name);
        child.appendChild(doc.createTextNode(QLatin1String("true")));
        fontElement.appendChild(child);
    } else {
        if (value == QLatin1String("false") || value == QLatin1String("0")) {
            QDomElement child = doc.createElement(name);
            child.appendChild(doc.createTextNode(QLatin1String("false")));
            fontElement.appendChild(child);
        }
    }
}

QDomElement Ui3Reader::findDerivedFontProperties(const QDomElement &n) const
{
    bool italic = false;
    bool bold = false;
    bool underline = false;
    bool strikeout = false;
    bool family = false;
    bool pointsize = false;

    QDomDocument doc = n.ownerDocument();
    QDomElement result = doc.createElement(QLatin1String("font"));

    QDomNode pn = n.parentNode();
    while (!pn.isNull()) {
        for (QDomElement e = pn.firstChild().toElement(); !e.isNull(); e = e.nextSibling().toElement()) {
            if (e.tagName().toLower() == QLatin1String("property") &&
                e.attribute(QLatin1String("name")) == QLatin1String("font")) {
                QDomElement f = e.firstChild().toElement();
                for (QDomElement fp = f.firstChild().toElement(); !fp.isNull(); fp = fp.nextSibling().toElement()) {
                    QString name = fp.tagName().toLower();
                    QString text = fp.text();
                    if (!italic && name == QLatin1String("italic")) {
                        italic = true;
                        addBooleanFontSubProperty(doc, name, text, result);
                    } else if (!bold && name == QLatin1String("bold")) {
                        bold = true;
                        addBooleanFontSubProperty(doc, name, text, result);
                    } else if (!underline && name == QLatin1String("underline")) {
                        underline = true;
                        addBooleanFontSubProperty(doc, name, text, result);
                    } else if (!strikeout && name == QLatin1String("strikeout")) {
                        strikeout = true;
                        addBooleanFontSubProperty(doc, name, text, result);
                    } else if (!family && name == QLatin1String("family")) {
                        family = true;
                        QDomElement child = doc.createElement(name);
                        child.appendChild(doc.createTextNode(text));
                        result.appendChild(child);
                    } else if (!pointsize && name == QLatin1String("pointsize")) {
                        pointsize = true;
                        QDomElement child = doc.createElement(name);
                        child.appendChild(doc.createTextNode(text));
                        result.appendChild(child);
                    }
                }
            }
        }
        pn = pn.parentNode();
    }

    return result;
}

void Ui3Reader::createProperties(const QDomElement &n, QList<DomProperty*> *properties,
                                 const QString &className)
{
    QString objectName;

    bool wordWrapFound = false;

    for (QDomElement e=n.firstChild().toElement(); !e.isNull(); e = e.nextSibling().toElement()) {
        if (e.tagName().toLower() == QLatin1String("property")) {
            QString name = e.attribute(QLatin1String("name"));

            // changes in QPalette
            if (name == QLatin1String("colorGroup")
                || name == QLatin1String("paletteForegroundColor")
                || name == QLatin1String("paletteBackgroundColor")
                || name == QLatin1String("backgroundMode")
                || name == QLatin1String("backgroundOrigin")
                || name == QLatin1String("paletteBackgroundPixmap")
                || name == QLatin1String("backgroundBrush")) {
                errorInvalidProperty(name, objectName, className, n.lineNumber(), n.columnNumber());
                continue;
            }

            // changes in QFrame
            if (name == QLatin1String("contentsRect")) {
                errorInvalidProperty(name, objectName, className, n.lineNumber(), n.columnNumber());
                continue;
            }

            // changes in QWidget
            if (name == QLatin1String("underMouse")
                || name == QLatin1String("ownFont")) {
                errorInvalidProperty(name, objectName, className, n.lineNumber(), n.columnNumber());
                continue;
            }

            if (name == QLatin1String("font")) {
                QDomElement f = e.firstChild().toElement();
                e.appendChild(findDerivedFontProperties(f));
                e.removeChild(f);
            }

            DomProperty *prop = readProperty(e);
            if (!prop)
                continue;

            if (prop->kind() == DomProperty::String) {
                QDomNodeList comments = e.elementsByTagName(QLatin1String("comment"));
                if (comments.length()) {
                    QString comment = comments.item(0).firstChild().toText().data();
                    if (!comment.isEmpty())
                        prop->elementString()->setAttributeComment(comment);
                }
            }

            // objectName
            if (name == QLatin1String("name")) {
                objectName = prop->elementCstring();
                continue;
            }

            if (className == QLatin1String("Line")
                && prop->attributeName() == QLatin1String("orientation")) {
                delete prop;
                continue;
            }

            if (className.mid(1) == QLatin1String("LineEdit")) {
                if (name == QLatin1String("hasMarkedText")) {
                    prop->setAttributeName(QLatin1String("hasSelectedText"));
                } else if (name == QLatin1String("edited")) {
                    prop->setAttributeName(QLatin1String("modified"));
                } else if (name == QLatin1String("markedText")) {
                    prop->setAttributeName(QLatin1String("selectedText"));
                }
            }

            if (className.endsWith(QLatin1String("ComboBox"))) {
                CONVERT_PROPERTY(QLatin1String("currentItem"), QLatin1String("currentIndex"));
                CONVERT_PROPERTY(QLatin1String("insertionPolicy"), QLatin1String("insertPolicy"));
            }

            if (className == QLatin1String("QToolBar")) {
                if (name == QLatin1String("label")) {
                    prop->setAttributeName(QLatin1String("windowTitle"));
                }
            }

            CONVERT_PROPERTY(QLatin1String("customWhatsThis"), QLatin1String("whatsThis"));
            CONVERT_PROPERTY(QLatin1String("icon"), QLatin1String("windowIcon"));
            CONVERT_PROPERTY(QLatin1String("iconText"), QLatin1String("windowIconText"));
            CONVERT_PROPERTY(QLatin1String("caption"), QLatin1String("windowTitle"));

            if (name == QLatin1String("name")) {
                continue; // skip the property name
            }

            if (name == QLatin1String("accel")) {
                prop->setAttributeName(QLatin1String("shortcut"));
            }

            CONVERT_PROPERTY(QLatin1String("pixmap"), QLatin1String("icon"));
            CONVERT_PROPERTY(QLatin1String("iconSet"), QLatin1String("icon"));
            CONVERT_PROPERTY(QLatin1String("textLabel"), QLatin1String("text"));

            CONVERT_PROPERTY(QLatin1String("toggleButton"), QLatin1String("checkable"));
            CONVERT_PROPERTY(QLatin1String("on"), QLatin1String("checked"));

            CONVERT_PROPERTY(QLatin1String("maxValue"), QLatin1String("maximum"));
            CONVERT_PROPERTY(QLatin1String("minValue"), QLatin1String("minimum"));
            CONVERT_PROPERTY(QLatin1String("lineStep"), QLatin1String("singleStep"));

            // QSlider
            CONVERT_PROPERTY(QLatin1String("tickmarks"), QLatin1String("tickPosition"));

            name = prop->attributeName(); // sync the name

            if (className == QLatin1String("QLabel") && name == QLatin1String("alignment")) {
                QString v = prop->elementSet();

                if (v.contains(QRegExp(QLatin1String("\\bWordBreak\\b"))))
                    wordWrapFound = true;
            }


            // resolve the flags and enumerator
            if (prop->kind() == DomProperty::Set) {
                QStringList flags = prop->elementSet().split(QLatin1Char('|'));
                QStringList v;
                foreach (QString fl, flags) {
                    QString e = WidgetInfo::resolveEnumerator(className, fl);
                    if (e.isEmpty()) {
                        e = m_porting->renameEnumerator(className + QLatin1String("::") + fl);
                    }

                    if (e.isEmpty()) {
                        fprintf(stderr, "uic3: flag '%s' for widget '%s' is not supported\n", fl.latin1(), className.latin1());
                        continue;
                    }

                    v.append(e);
                }

                if (v.isEmpty()) {
                    delete prop;
                    continue;
                }

                prop->setElementSet(v.join(QLatin1String("|")));
            } else if (prop->kind() == DomProperty::Enum) {
                QString e = WidgetInfo::resolveEnumerator(className, prop->elementEnum());
                if (e.isEmpty()) {
                    e = m_porting->renameEnumerator(className + QLatin1String("::") + prop->elementEnum());
                }

                if (e.isEmpty()) {
                    fprintf(stderr, "uic3: enumerator '%s' for widget '%s' is not supported\n",
                            prop->elementEnum().latin1(), className.latin1());

                    delete prop;
                    continue;
                }
                prop->setElementEnum(e);
            }


            if (className.size()
                && !(className == QLatin1String("QLabel") && name == QLatin1String("buddy"))
                && !(name == QLatin1String("buttonGroupId"))
                && !(name == QLatin1String("frameworkCode"))
                && !(name == QLatin1String("database"))) {
                if (!WidgetInfo::isValidProperty(className, name)) {
                    errorInvalidProperty(name, objectName, className, n.lineNumber(), n.columnNumber());
                    delete prop;
                } else {
                    properties->append(prop);
                }
            } else {
                properties->append(prop);
            }
        }
    }
    if (className == QLatin1String("QLabel")) {
        DomProperty *wordWrap = new DomProperty();
        wordWrap->setAttributeName(QLatin1String("wordWrap"));
        if (wordWrapFound)
            wordWrap->setElementBool(QLatin1String("true"));
        else
            wordWrap->setElementBool(QLatin1String("false"));
        properties->append(wordWrap);
    }
}

static int toQt4SizePolicy(int qt3SizePolicy)
{
    if (qt3SizePolicy == 2) // qt 3 Ignored value
        return QSizePolicy::Ignored;
    return qt3SizePolicy;
}

DomProperty *Ui3Reader::readProperty(const QDomElement &e)
{
    QString name = e.firstChild().toElement().tagName().toLower();

    if (name == QLatin1String("class")) // skip class
        name = e.firstChild().nextSibling().toElement().tagName().toLower();

    DomProperty *p = new DomProperty;
    p->read(e);

    if (p->kind() == DomProperty::Number) {
        QString value = e.firstChild().toElement().firstChild().nodeValue();

        if (value.contains(QLatin1Char('.'))) {
            p->setElementDouble(value.toDouble());
        }
    } else if (p->kind() == DomProperty::Pixmap) {
        DomResourcePixmap *domPix = p->elementPixmap();
        if (m_extractImages) {
            QString imageFile = domPix->text() + QLatin1String(".xpm");
            if (m_imageMap.contains(domPix->text()))
                imageFile = m_imageMap.value(domPix->text());
            domPix->setAttributeResource(m_qrcOutputFile);
            domPix->setText(QLatin1String(":/") + nameOfClass + QLatin1String("/images/") + imageFile);
        }
    } else if (p->kind() == DomProperty::SizePolicy) {
        DomSizePolicy *sp = p->elementSizePolicy();
        if (sp) {
            if (sp->hasElementHSizeType())
                sp->setElementHSizeType(toQt4SizePolicy(sp->elementHSizeType()));
            if (sp->hasElementVSizeType())
                sp->setElementVSizeType(toQt4SizePolicy(sp->elementVSizeType()));
        }
    } else if (p->kind() == DomProperty::Unknown) {
        delete p;
        p = 0;
    }

    return p;
}

void Ui3Reader::createAttributes(const QDomElement &n, QList<DomProperty*> *properties,
                                 const QString &className)
{
    Q_UNUSED(className);

    for (QDomElement e=n.firstChild().toElement(); !e.isNull(); e = e.nextSibling().toElement()) {
        if (e.tagName().toLower() == QLatin1String("attribute")) {
            QString name = e.attribute(QLatin1String("name"));

            DomProperty *prop = readProperty(e);
            if (!prop)
                continue;

            properties->append(prop);
        }
    }
}

QString Ui3Reader::fixDeclaration(const QString &d) const
{
    QString text;

    int i = 0;
    while (i < d.size()) {
        QChar ch = d.at(i);

        if (ch.isLetter() || ch == QLatin1Char('_')) {
            int start = i;
            while (i < d.size() && (d.at(i).isLetterOrNumber() || d.at(i) == QLatin1Char('_')))
                ++i;

            text += fixClassName(d.mid(start, i-start));
        } else {
            text += ch;
            ++i;
        }
    }

    return text;
}

/*
    fixes a (possible composite) type name
*/
QString Ui3Reader::fixType(const QString &t) const
{
    QString newText = t;
    //split type name on <>*& and whitespace
    QStringList typeNames = t.split(QRegExp(QLatin1String("<|>|\\*|&| ")), QString::SkipEmptyParts);
    foreach(QString typeName , typeNames) {
        QString newName = fixClassName(typeName);
        if( newName != typeName ) {
            newText.replace(typeName, newName);
        }
    }
    return newText;
}

QString Ui3Reader::fixMethod(const QString &method) const
{
    const QByteArray normalized = QMetaObject::normalizedSignature(method.toLatin1());
    QByteArray result;
    int index = normalized.indexOf('(');
    if (index == -1)
        return QLatin1String(normalized);
    result.append(normalized.left(++index));
    int limit = normalized.length()-1;
    while (index < limit) {
        QByteArray type;
        while ((index < limit) && (normalized.at(index) != ','))
            type.append(normalized.at(index++));
        result.append(fixType(QLatin1String(type)).toLatin1());
        if ((index < limit) && (normalized.at(index) == ','))
            result.append(normalized.at(index++));
    }
    result.append(normalized.mid(index));
    return QLatin1String(result);
}

QT_END_NAMESPACE
