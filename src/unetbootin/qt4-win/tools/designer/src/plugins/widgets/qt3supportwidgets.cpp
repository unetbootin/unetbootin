/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
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

#include "q3toolbar/q3toolbar_plugin.h"
#include "q3iconview/q3iconview_plugin.h"
#include "q3groupbox/q3groupbox_plugin.h"
#include "q3frame/q3frame_plugin.h"
#include "q3wizard/q3wizard_plugin.h"
#include "q3mainwindow/q3mainwindow_plugin.h"
#include "q3widgetstack/q3widgetstack_plugin.h"
#include "q3buttongroup/q3buttongroup_plugin.h"
#include "q3listview/q3listview_plugin.h"
#include "q3table/q3table_plugin.h"
#include "q3listbox/q3listbox_plugin.h"
#include "q3listview/q3listview_plugin.h"
#include "q3textedit/q3textedit_plugin.h"
#include "q3dateedit/q3dateedit_plugin.h"
#include "q3timeedit/q3timeedit_plugin.h"
#include "q3datetimeedit/q3datetimeedit_plugin.h"
#include "q3progressbar/q3progressbar_plugin.h"
#include "q3textbrowser/q3textbrowser_plugin.h"

#include <QtDesigner/QDesignerCustomWidgetCollectionInterface>
#include <QtCore/qplugin.h>
#include <QtCore/qdebug.h>
#include <QtGui/QIcon>

QT_BEGIN_NAMESPACE

class Qt3SupportWidgets: public QObject, public QDesignerCustomWidgetCollectionInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)
public:
    Qt3SupportWidgets(QObject *parent = 0);

    virtual QList<QDesignerCustomWidgetInterface*> customWidgets() const;

private:
    QList<QDesignerCustomWidgetInterface*> m_plugins;
};

Qt3SupportWidgets::Qt3SupportWidgets(QObject *parent)
    : QObject(parent)
{
    const QIcon qt3Icon(QLatin1String(":/trolltech/formeditor/images/qt3logo.png"));
    m_plugins.append(new Q3ToolBarPlugin(qt3Icon, this));
    m_plugins.append(new Q3IconViewPlugin(qt3Icon, this));
    m_plugins.append(new Q3GroupBoxPlugin(qt3Icon, this));
    m_plugins.append(new Q3FramePlugin(qt3Icon, this));
    m_plugins.append(new Q3WizardPlugin(qt3Icon, this));
    m_plugins.append(new Q3MainWindowPlugin(qt3Icon, this));
    m_plugins.append(new Q3WidgetStackPlugin(qt3Icon, this));
    m_plugins.append(new Q3ButtonGroupPlugin(qt3Icon, this));
    m_plugins.append(new Q3TablePlugin(qt3Icon, this));
    m_plugins.append(new Q3ListBoxPlugin(qt3Icon, this));
    m_plugins.append(new Q3ListViewPlugin(qt3Icon, this));
    m_plugins.append(new Q3TextEditPlugin(qt3Icon, this));
    m_plugins.append(new Q3DateEditPlugin(qt3Icon, this));
    m_plugins.append(new Q3TimeEditPlugin(qt3Icon, this));
    m_plugins.append(new Q3DateTimeEditPlugin(qt3Icon, this));
    m_plugins.append(new Q3ProgressBarPlugin(qt3Icon, this));
    m_plugins.append(new Q3TextBrowserPlugin(qt3Icon, this));
}

QList<QDesignerCustomWidgetInterface*> Qt3SupportWidgets::customWidgets() const
{
    return m_plugins;
}

Q_EXPORT_PLUGIN(Qt3SupportWidgets)

QT_END_NAMESPACE

#include "qt3supportwidgets.moc"
