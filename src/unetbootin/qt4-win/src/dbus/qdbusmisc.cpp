/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtDBus module of the Qt Toolkit.
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

#include <string.h>

#include <QtCore/qvariant.h>
#include <QtCore/qmetaobject.h>

#include "qdbusutil_p.h"
#include "qdbusconnection_p.h"
#include "qdbusmetatype_p.h"

QT_BEGIN_NAMESPACE

bool qDBusCheckAsyncTag(const char *tag)
{
    static const char noReplyTag[] = "Q_NOREPLY";
    if (!tag || !*tag)
        return false;

    const char *p = strstr(tag, noReplyTag);
    if (p != NULL &&
        (p == tag || *(p-1) == ' ') &&
        (p[sizeof noReplyTag - 1] == '\0' || p[sizeof noReplyTag - 1] == ' '))
        return true;

    return false;
}

int qDBusNameToTypeId(const char *name)
{
    int id = static_cast<int>( QVariant::nameToType(name) );
    if (id == QVariant::UserType)
        id = QMetaType::type(name);
    return id;
}

// calculates the metatypes for the method
// the slot must have the parameters in the following form:
//  - zero or more value or const-ref parameters of any kind
//  - zero or one const ref of QDBusMessage
//  - zero or more non-const ref parameters
// No parameter may be a template.
// this function returns -1 if the parameters don't match the above form
// this function returns the number of *input* parameters, including the QDBusMessage one if any
// this function does not check the return type, so metaTypes[0] is always 0 and always present
// metaTypes.count() >= retval + 1 in all cases
//
// sig must be the normalised signature for the method
int qDBusParametersForMethod(const QMetaMethod &mm, QList<int>& metaTypes)
{
    QDBusMetaTypeId::init();

    QList<QByteArray> parameterTypes = mm.parameterTypes();
    metaTypes.clear();

    metaTypes.append(0);        // return type
    int inputCount = 0;
    bool seenMessage = false;
    QList<QByteArray>::ConstIterator it = parameterTypes.constBegin();
    QList<QByteArray>::ConstIterator end = parameterTypes.constEnd();
    for ( ; it != end; ++it) {
        const QByteArray &type = *it;
        if (type.endsWith('*')) {
            //qWarning("Could not parse the method '%s'", mm.signature());
            // pointer?
            return -1;
        }

        if (type.endsWith('&')) {
            QByteArray basictype = type;
            basictype.truncate(type.length() - 1);

            int id = qDBusNameToTypeId(basictype);
            if (id == 0) {
                //qWarning("Could not parse the method '%s'", mm.signature());
                // invalid type in method parameter list
                return -1;
            } else if (QDBusMetaType::typeToSignature(id) == 0)
                return -1;

            metaTypes.append( id );
            seenMessage = true; // it cannot appear anymore anyways
            continue;
        }

        if (seenMessage) {      // && !type.endsWith('&')
            //qWarning("Could not parse the method '%s'", mm.signature());
            // non-output parameters after message or after output params
            return -1;          // not allowed
        }

        int id = qDBusNameToTypeId(type);
        if (id == 0) {
            //qWarning("Could not parse the method '%s'", mm.signature());
            // invalid type in method parameter list
            return -1;
        }

        if (id == QDBusMetaTypeId::message)
            seenMessage = true;
        else if (QDBusMetaType::typeToSignature(id) == 0)
            return -1;

        metaTypes.append(id);
        ++inputCount;
    }

    return inputCount;
}

QT_END_NAMESPACE
