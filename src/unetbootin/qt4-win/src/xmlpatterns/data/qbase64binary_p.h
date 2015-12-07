/****************************************************************************
**
** Copyright (C) 2006-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtXMLPatterns module of the Qt Toolkit.
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Patternist_Base64Binary_H
#define Patternist_Base64Binary_H

#include "qitem_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{

    /**
     * @short Implements the value instance of the @c xs:base64Binary type.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_xdm
     */
    class Base64Binary : public AtomicValue
    {
    public:
        friend class CommonValues;

        typedef AtomicValue::Ptr Ptr;

        /**
         * Creates an instance representing @p value.
         */
        static AtomicValue::Ptr fromLexical(const QString &value);

        static Base64Binary::Ptr fromValue(const QByteArray &data);

        virtual QString stringValue() const;
        virtual ItemType::Ptr type() const;
        inline const QByteArray &asByteArray() const
        {
            return m_value;
        }

    protected:
        Base64Binary(const QByteArray &val);

        const QByteArray m_value;

    private:
        /**
         * @short Assumes @p in is a lexical representation of @c xs:base64Binary, and
         * converts it to the binary data set in @p out.
         *
         * If @p instr is invalid Base64 content, @p ok is set to
         * false, and the returned QByteArray has an undefined value.
         *
         *  We cannot use QByteArray::fromBase64() because it doesn't do the
         *  necessary validation that we need to properly implement W3C XML
         *  Schema's xs:base64Binary type.
         */
        static void base64Decode(const QByteArray &in, QByteArray &out, bool &ok);

        static const char Base64DecMap[128];
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
