/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtXMLPatterns module of the Qt Toolkit.
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
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Patternist_HexBinary_H
#define Patternist_HexBinary_H

#include "qbase64binary_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{

    /**
     * @short Implements the value instance of the @c xs:hexBinary type.
     *
     * HexBinary inherits from Base64Binary for implementation reasons. The two
     * classes are similar, and inheritance therefore save code.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_xdm
     * @todo Documentation is missing
     */
    class HexBinary : public Base64Binary
    {
    public:
        friend class CommonValues;

        typedef AtomicValue::Ptr Ptr;

        virtual QString stringValue() const;
        virtual ItemType::Ptr type() const;

        /**
         * Creates a @c xs:hexBinary from the lexical representation @p value.
         */
        static AtomicValue::Ptr fromLexical(const NamePool::Ptr &np, const QString &value);

        /**
         * Creates an instance representing @p value.
         */
        static HexBinary::Ptr fromValue(const QByteArray &data);

    protected:
        HexBinary(const QByteArray &val);

    private:
        /**
         * @short Returns -1 on invalid input.
         */
        static inline qint8 fromHex(const QChar &c);
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
