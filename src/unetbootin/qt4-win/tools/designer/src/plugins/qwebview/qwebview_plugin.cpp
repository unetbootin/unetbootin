/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
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

#include "qwebview_plugin.h"

#include <QtDesigner/QExtensionFactory>
#include <QtDesigner/QExtensionManager>

#include <QtCore/qplugin.h>
#include <QtWebKit/QWebView>

static const char *toolTipC = "QtWebKit Web widget";

QT_BEGIN_NAMESPACE

QWebViewPlugin::QWebViewPlugin(QObject *parent) :
    QObject(parent),
    m_initialized(false)
{
}

QString QWebViewPlugin::name() const
{
    return QLatin1String("QWebView");
}

QString QWebViewPlugin::group() const
{
    return QLatin1String("Display Widgets");
}

QString QWebViewPlugin::toolTip() const
{
    return QString(QLatin1String(toolTipC));
}

QString QWebViewPlugin::whatsThis() const
{
    return QString(QLatin1String(toolTipC));
}

QString QWebViewPlugin::includeFile() const
{
    return QLatin1String("QtWebKit/QWebView");
}

QIcon QWebViewPlugin::icon() const
{
    return QIcon(QLatin1String(":/trolltech/qwebview/images/qwebview.png"));
}

bool QWebViewPlugin::isContainer() const
{
    return false;
}

QWidget *QWebViewPlugin::createWidget(QWidget *parent)
{
    return new QWebView(parent);
}

bool QWebViewPlugin::isInitialized() const
{
    return m_initialized;
}

void QWebViewPlugin::initialize(QDesignerFormEditorInterface * /*core*/)
{
    if (m_initialized)
        return;

    m_initialized = true;
}

QString QWebViewPlugin::domXml() const
{
    return QLatin1String("\
    <widget class=\"QWebView\" name=\"webView\">\
        <property name=\"url\">\
            <url>\
                <string>about:blank</string>\
            </url>\
        </property>\
        <property name=\"geometry\">\
            <rect>\
                <x>0</x>\
                <y>0</y>\
                <width>300</width>\
                <height>200</height>\
            </rect>\
        </property>\
    </widget>\
    ");
}

Q_EXPORT_PLUGIN2(customwidgetplugin, QWebViewPlugin)

QT_END_NAMESPACE
