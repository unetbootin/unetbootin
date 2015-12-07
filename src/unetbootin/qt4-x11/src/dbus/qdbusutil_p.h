/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDBus module of the Qt Toolkit.
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QLibrary class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#ifndef QDBUSUTIL_H
#define QDBUSUTIL_H

#include <QtCore/qstring.h>
#include <QtCore/qvariant.h>

#include <QtDBus/qdbusmacros.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QDBusUtil
{
    QDBUS_EXPORT bool isValidInterfaceName(const QString &ifaceName);

    QDBUS_EXPORT bool isValidUniqueConnectionName(const QString &busName);

    QDBUS_EXPORT bool isValidBusName(const QString &busName);

    QDBUS_EXPORT bool isValidMemberName(const QString &memberName);

    QDBUS_EXPORT bool isValidErrorName(const QString &errorName);

    QDBUS_EXPORT bool isValidPartOfObjectPath(const QString &path);

    QDBUS_EXPORT bool isValidObjectPath(const QString &path);

    QDBUS_EXPORT bool isValidSignature(const QString &signature);

    QDBUS_EXPORT bool isValidSingleSignature(const QString &signature);

    QDBUS_EXPORT QString argumentToString(const QVariant &variant);
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
