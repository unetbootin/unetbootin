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

#include "qresource.h"
#include "qresource_iterator_p.h"

#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

QResourceFileEngineIterator::QResourceFileEngineIterator(QDir::Filters filters,
                                                         const QStringList &filterNames)
    : QAbstractFileEngineIterator(filters, filterNames), index(-1)
{
}

QResourceFileEngineIterator::~QResourceFileEngineIterator()
{
}

QString QResourceFileEngineIterator::next()
{
    if (!hasNext())
        return QString();
    ++index;
    return currentFilePath();
}

bool QResourceFileEngineIterator::hasNext() const
{
    if (index == -1) {
        // Lazy initialization of the iterator
        QResource resource(path());
        if (!resource.isValid())
            return false;

        // Initialize and move to the next entry.
        QResourceFileEngineIterator *that = const_cast<QResourceFileEngineIterator *>(this);
        that->entries = resource.children();
        if (!that->entries.isEmpty())
            that->index = 0;
    }

    return index <= entries.size();
}

QString QResourceFileEngineIterator::currentFileName() const
{
    if (index <= 0 || index > entries.size())
        return QString();
    return entries.at(index - 1);
}

QT_END_NAMESPACE
