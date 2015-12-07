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

#include "q3toolbar_plugin.h"
#include "q3toolbar_extrainfo.h"

#include <QtDesigner/QExtensionFactory>
#include <QtDesigner/QExtensionManager>

#include <QtCore/qplugin.h>
#include <QtGui/QIcon>
#include <QtCore/QDebug>

#include <Qt3Support/Q3MainWindow>
#include <Qt3Support/Q3ToolBar>
#include <QtGui/QMainWindow>
#include <QtGui/QToolBar>

QT_BEGIN_NAMESPACE

Q3ToolBarPlugin::Q3ToolBarPlugin(const QIcon &icon, QObject *parent)
        : QObject(parent), m_initialized(false), m_icon(icon)
{}

QString Q3ToolBarPlugin::name() const
{ return QLatin1String("Q3ToolBar"); }

QString Q3ToolBarPlugin::group() const
{ return QLatin1String("Qt 3 Support"); }

QString Q3ToolBarPlugin::toolTip() const
{ return QString(); }

QString Q3ToolBarPlugin::whatsThis() const
{ return QString(); }

QString Q3ToolBarPlugin::includeFile() const
{ return QLatin1String("q3listview.h"); }

QIcon Q3ToolBarPlugin::icon() const
{ return m_icon; }

bool Q3ToolBarPlugin::isContainer() const
{ return false; }

QWidget *Q3ToolBarPlugin::createWidget(QWidget *parent)
{
    if (!parent)
        return new Q3ToolBar;
    // If there is a parent, it must be a Q3MainWindow
    if (Q3MainWindow *mw3 = qobject_cast<Q3MainWindow*>(parent))
        return new Q3ToolBar(mw3);
    // Somebody hacked up a form?
    if (QMainWindow *mw4 = qobject_cast<QMainWindow*>(parent)) {
        qDebug() << "*** WARNING QMainWindow was passed as a parent widget of Q3ToolBar. Creating a QToolBar...";
        return new QToolBar(mw4);
    }
    // Can't be helped
    const QString msg = QString::fromUtf8("*** WARNING Parent widget of Q3ToolBar must be a Q3MainWindow (%1)!").arg(QLatin1String(parent->metaObject()->className()));
    qDebug() << msg;
    return 0;
}

bool Q3ToolBarPlugin::isInitialized() const
{ return m_initialized; }

void Q3ToolBarPlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);

    if (m_initialized)
        return;

    QExtensionManager *mgr = core->extensionManager();
    Q_ASSERT(mgr != 0);

    mgr->registerExtensions(new Q3ToolBarExtraInfoFactory(core, mgr), Q_TYPEID(QDesignerExtraInfoExtension));

    m_initialized = true;
}

QString Q3ToolBarPlugin::codeTemplate() const
{ return QString(); }

QString Q3ToolBarPlugin::domXml() const
{ return QString(); }



QT_END_NAMESPACE
