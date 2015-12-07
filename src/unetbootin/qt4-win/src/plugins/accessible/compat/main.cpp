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

#include "qaccessiblecompat.h"
#include "q3simplewidgets.h"
#include "q3complexwidgets.h"

#include <qaccessibleplugin.h>
#include <qplugin.h>
#include <qstringlist.h>
#include <q3toolbar.h>

QT_BEGIN_NAMESPACE

class CompatAccessibleFactory : public QAccessiblePlugin
{
public:
    CompatAccessibleFactory();

    QStringList keys() const;
    QAccessibleInterface *create(const QString &classname, QObject *object);
};

CompatAccessibleFactory::CompatAccessibleFactory()
{
}

QStringList CompatAccessibleFactory::keys() const
{
    QStringList list;
    list << QLatin1String("Q3TextEdit");
    list << QLatin1String("Q3IconView");
    list << QLatin1String("Q3ListView");
    list << QLatin1String("Q3WidgetStack");
    list << QLatin1String("Q3GroupBox");
    list << QLatin1String("Q3ToolBar");
    list << QLatin1String("Q3ToolBarSeparator");
    list << QLatin1String("Q3DockWindowHandle");
    list << QLatin1String("Q3DockWindowResizeHandle");
    list << QLatin1String("Q3MainWindow");
    list << QLatin1String("Q3Header");
    list << QLatin1String("Q3ListBox");
    list << QLatin1String("Q3Table");
    list << QLatin1String("Q3TitleBar");

    return list;
}

QAccessibleInterface *CompatAccessibleFactory::create(const QString &classname, QObject *object)
{
    QAccessibleInterface *iface = 0;
    if (!object || !object->isWidgetType())
        return iface;
    QWidget *widget = static_cast<QWidget*>(object);

    if (classname == QLatin1String("Q3TextEdit")) {
        iface = new Q3AccessibleTextEdit(widget);
    } else if (classname == QLatin1String("Q3IconView")) {
        iface = new QAccessibleIconView(widget);
    } else if (classname == QLatin1String("Q3ListView")) {
        iface = new QAccessibleListView(widget);
    } else if (classname == QLatin1String("Q3WidgetStack")) {
        iface = new QAccessibleWidgetStack(widget);
    } else if (classname == QLatin1String("Q3ListBox")) {
        iface = new QAccessibleListBox(widget);
    } else if (classname == QLatin1String("Q3Table")) {
        iface = new Q3AccessibleScrollView(widget, Table);
    } else if (classname == QLatin1String("Q3GroupBox")) {
        iface = new Q3AccessibleDisplay(widget, Grouping);
    } else if (classname == QLatin1String("Q3ToolBar")) {
        iface = new QAccessibleWidget(widget, ToolBar, static_cast<Q3ToolBar *>(widget)->label());
    } else if (classname == QLatin1String("Q3MainWindow")) {
        iface = new QAccessibleWidget(widget, Application);
    } else if (classname == QLatin1String("Q3ToolBarSeparator")) {
        iface = new QAccessibleWidget(widget, Separator);
    } else if (classname == QLatin1String("Q3DockWindowHandle")) {
        iface = new QAccessibleWidget(widget, Grip);
    } else if (classname == QLatin1String("Q3DockWindowResizeHandle")) {
        iface = new QAccessibleWidget(widget, Grip);
    } else if (classname == QLatin1String("Q3Header")) {
        iface = new Q3AccessibleHeader(widget);
    } else if (classname == QLatin1String("Q3TitleBar")) {
        iface = new Q3AccessibleTitleBar(widget);
    }

    return iface;
}

Q_EXPORT_STATIC_PLUGIN(CompatAccessibleFactory)
Q_EXPORT_PLUGIN2(qtaccessiblecompatwidgets, CompatAccessibleFactory)

QT_END_NAMESPACE
