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

#ifndef Patternist_SourceLocationReflection_H
#define Patternist_SourceLocationReflection_H

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QString;

namespace QPatternist
{
    /**
     * @short Base class for all instances that represents something
     * at a certain location.
     *
     * SourceLocationReflection does not provide the source location itself,
     * the address to an instance is the mark for it, that in turn can be used
     * for looking up the source location where that mapping is provided.
     *
     * However, this SourceLocationReflection is not itself the mark. The real
     * mark is retrieved by calling actualReflection(). This mechanism
     * allows a SourceLocationReflection sub-class to delegate, or be an alias,
     * for another source location mark.
     *
     * If sourceLocation() returns a non-null object, it will be used instead
     * of looking up via actualReflection().
     *
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_expressions
     */
    class Q_AUTOTEST_EXPORT SourceLocationReflection
    {
    public:
        inline SourceLocationReflection()
        {
        }

        virtual ~SourceLocationReflection()
        {
        }

        virtual const SourceLocationReflection *actualReflection() const = 0;

        /**
         * A description of what represents the source code location, for
         * human consumption. Must be translated, as appropriate.
         */
        virtual QString description() const
        {
            return QString();
        }

        virtual QSourceLocation sourceLocation() const;

    private:
        Q_DISABLE_COPY(SourceLocationReflection)
    };

    class DelegatingSourceLocationReflection : public SourceLocationReflection
    {
    public:
        inline DelegatingSourceLocationReflection(const SourceLocationReflection *const r) : m_r(r)
        {
            Q_ASSERT(r);
        }

        virtual const SourceLocationReflection *actualReflection() const;
        virtual QString description() const;

    private:
        const SourceLocationReflection *const m_r;
    };

}

QT_END_NAMESPACE

QT_END_HEADER

#endif
