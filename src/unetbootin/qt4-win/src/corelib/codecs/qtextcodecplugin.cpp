/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "qtextcodecplugin.h"
#include "qstringlist.h"

#ifndef QT_NO_TEXTCODECPLUGIN

QT_BEGIN_NAMESPACE

/*!
    \class QTextCodecPlugin
    \brief The QTextCodecPlugin class provides an abstract base for custom QTextCodec plugins.
    \reentrant
    \ingroup plugins

    The text codec plugin is a simple plugin interface that makes it
    easy to create custom text codecs that can be loaded dynamically
    into applications.

    Writing a text codec plugin is achieved by subclassing this base
    class, reimplementing the pure virtual functions names(),
    aliases(), createForName(), mibEnums() and createForMib(), and
    exporting the class with the Q_EXPORT_PLUGIN2() macro. See \l{How
    to Create Qt Plugins} for details.

    See the \l{http://www.iana.org/assignments/character-sets}{IANA
    character-sets encoding file} for more information on mime
    names and mib enums.
*/

/*!
    \fn QStringList QTextCodecPlugin::names() const

    Returns the list of MIME names supported by this plugin.

    If a codec has several names, the extra names are returned by aliases().

    \sa createForName(), aliases()
*/

/*!
    \fn QList<QByteArray> QTextCodecPlugin::aliases() const

    Returns the list of aliases supported by this plugin.
*/

/*!
    \fn QTextCodec *QTextCodecPlugin::createForName(const QByteArray &name)

    Creates a QTextCodec object for the codec called \a name. The \a name
    must come from the list of encodings returned by names(). Encoding
    names are case sensitive.

    Example:

    \snippet doc/src/snippets/code/src.corelib.codecs.qtextcodecplugin.cpp 0

    \sa names()
*/


/*!
    \fn QList<int> QTextCodecPlugin::mibEnums() const

    Returns the list of mib enums supported by this plugin.

    \sa createForMib()
*/

/*!
    \fn QTextCodec *QTextCodecPlugin::createForMib(int mib);

    Creates a QTextCodec object for the mib enum \a mib.

    See \l{http://www.iana.org/assignments/character-sets}{the
    IANA character-sets encoding file} for more information.

    \sa mibEnums()
*/

/*!
    Constructs a text codec plugin with the given \a parent. This is
    invoked automatically by the Q_EXPORT_PLUGIN2() macro.
*/
QTextCodecPlugin::QTextCodecPlugin(QObject *parent)
    : QObject(parent)
{
}

/*!
    Destroys the text codec plugin.

    You never have to call this explicitly. Qt destroys a plugin
    automatically when it is no longer used.
*/
QTextCodecPlugin::~QTextCodecPlugin()
{
}

QStringList QTextCodecPlugin::keys() const
{
    QStringList keys;
    QList<QByteArray> list = names();
    list += aliases();
    for (int i = 0; i < list.size(); ++i)
        keys += QString::fromLatin1(list.at(i));
    QList<int> mibs = mibEnums();
    for (int i = 0; i < mibs.count(); ++i)
        keys += QLatin1String("MIB: ") + QString::number(mibs.at(i));
    return keys;
}

QTextCodec *QTextCodecPlugin::create(const QString &name)
{
    if (name.startsWith(QLatin1String("MIB: ")))
        return createForMib(name.mid(4).toInt());
    return createForName(name.toLatin1());
}

QT_END_NAMESPACE

#endif // QT_NO_TEXTCODECPLUGIN
