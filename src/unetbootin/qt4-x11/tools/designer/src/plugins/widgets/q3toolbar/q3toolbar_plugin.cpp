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
