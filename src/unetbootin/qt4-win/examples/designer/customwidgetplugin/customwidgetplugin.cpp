/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
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

#include "analogclock.h"
#include "customwidgetplugin.h"

#include <QtPlugin>

//! [0]
AnalogClockPlugin::AnalogClockPlugin(QObject *parent)
    : QObject(parent)
{
    initialized = false;
}
//! [0]

//! [1]
void AnalogClockPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (initialized)
        return;

    initialized = true;
}
//! [1]

//! [2]
bool AnalogClockPlugin::isInitialized() const
{
    return initialized;
}
//! [2]

//! [3]
QWidget *AnalogClockPlugin::createWidget(QWidget *parent)
{
    return new AnalogClock(parent);
}
//! [3]

//! [4]
QString AnalogClockPlugin::name() const
{
    return "AnalogClock";
}
//! [4]

//! [5]
QString AnalogClockPlugin::group() const
{
    return "Display Widgets [Examples]";
}
//! [5]

//! [6]
QIcon AnalogClockPlugin::icon() const
{
    return QIcon();
}
//! [6]

//! [7]
QString AnalogClockPlugin::toolTip() const
{
    return "";
}
//! [7]

//! [8]
QString AnalogClockPlugin::whatsThis() const
{
    return "";
}
//! [8]

//! [9]
bool AnalogClockPlugin::isContainer() const
{
    return false;
}
//! [9]

//! [10]
QString AnalogClockPlugin::domXml() const
{
    return "<widget class=\"AnalogClock\" name=\"analogClock\">\n"
//! [11]
           " <property name=\"geometry\">\n"
           "  <rect>\n"
           "   <x>0</x>\n"
           "   <y>0</y>\n"
           "   <width>100</width>\n"
           "   <height>100</height>\n"
           "  </rect>\n"
           " </property>\n"
//! [11]
           " <property name=\"toolTip\" >\n"
           "  <string>The current time</string>\n"
           " </property>\n"
           " <property name=\"whatsThis\" >\n"
           "  <string>The analog clock widget displays "
           "the current time.</string>\n"
           " </property>\n"
           "</widget>\n";
}
//! [10]

//! [12]
QString AnalogClockPlugin::includeFile() const
{
    return "analogclock.h";
}
//! [12]

//! [13]
Q_EXPORT_PLUGIN2(customwidgetplugin, AnalogClockPlugin)
//! [13]
