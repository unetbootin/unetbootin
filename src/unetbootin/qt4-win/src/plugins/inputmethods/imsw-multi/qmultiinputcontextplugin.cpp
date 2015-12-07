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

/****************************************************************************
**
** Implementation of QMultiInputContextPlugin class
**
** Copyright (C) 2004 immodule for Qt Project.  All rights reserved.
**
** This file is written to contribute to Trolltech ASA under their own
** license. You may use this file under your Qt license. Following
** description is copied from their original file headers. Contact
** immodule-qt@freedesktop.org if any conditions of this licensing are
** not clear to you.
**
****************************************************************************/

#ifndef QT_NO_IM
#include "qmultiinputcontext.h"
#include "qmultiinputcontextplugin.h"
#include <qinputcontextplugin.h>
#include <qstringlist.h>

QT_BEGIN_NAMESPACE

QMultiInputContextPlugin::QMultiInputContextPlugin()
{
}

QMultiInputContextPlugin::~QMultiInputContextPlugin()
{
}

QStringList QMultiInputContextPlugin::keys() const
{
    // input method switcher should named with "imsw-" prefix to
    // prevent to be listed in ordinary input method list.
    return QStringList( QLatin1String("imsw-multi") );
}

QInputContext *QMultiInputContextPlugin::create( const QString &key )
{
    if (key != QLatin1String("imsw-multi"))
        return 0;
    return new QMultiInputContext;
}

QStringList QMultiInputContextPlugin::languages( const QString & )
{
    return QStringList();
}

QString QMultiInputContextPlugin::displayName( const QString &key )
{
    if (key != QLatin1String("imsw-multi"))
        return QString();
    return tr( "Multiple input method switcher" );
}

QString QMultiInputContextPlugin::description( const QString &key )
{
    if (key != QLatin1String("imsw-multi"))
        return QString();
    return tr( "Multiple input method switcher that uses the context menu of the text widgets" );
}


Q_EXPORT_STATIC_PLUGIN(QMultiInputContextPlugin)
Q_EXPORT_PLUGIN2(qimsw_multi, QMultiInputContextPlugin)

QT_END_NAMESPACE

#endif
