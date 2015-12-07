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

#ifndef Patternist_ExternalVariableLoader_H
#define Patternist_ExternalVariableLoader_H

#include "qitem_p.h"
#include "qsequencetype_p.h"
#include "qxmlname.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    class DynamicContext;

    /**
     * @short Responsible for loading and declaring available external variables.
     *
     * An external variable in XQuery is a global variable that has been declared to receive
     * its value from the XQuery implementation, as opposed to an initializing expression. Here
     * is an example of a query with an external variable declaration, followed by a ordinary
     * global variable:
     *
     * <tt> declare variable $theName external;
     * declare variable $theName := "the value";
     * "And here's the query body(a string literal)"</tt>
     *
     * An external variable declaration can also specify a sequence type:
     *
     * <tt>declare variable $theName as xs:integer external;</tt>
     *
     * This class allows the user to supply the values to external variables. When
     * an external variable declaration occur in the query,
     * announceExternalVariable() is called.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class Q_AUTOTEST_EXPORT ExternalVariableLoader : public QSharedData
    {
    public:
        typedef QExplicitlySharedDataPointer<ExternalVariableLoader> Ptr;
        inline ExternalVariableLoader() {}

        virtual ~ExternalVariableLoader();

        /**
         * Called when Patternist encounters an external variable in the query. It is guaranteed
         * to be called once for each external variable appearing in a query module.
         *
         * @param name the name of the variable. Quaranteed to never be @c null.
         * @param declaredType the type that the user declared the variable to be of. Whether
         * this type matches the actual value of the variable or not is irrelevant. Patternist
         * will do the necessary error handling based on the sequence type that is returned from
         * this function. If the user didn't declare a type, the type is <tt>item()*</tt>(zero or
         * more items). Quaranteed to never be @c null.
         * @returns the sequence type of the value this ExternalVariableLoader actually supplies. However,
         * if the ExternalVariableLoader knows it cannot supply a variable by this name, @c null should be
         * returned.
         */
        virtual SequenceType::Ptr announceExternalVariable(const QXmlName name,
                                                           const SequenceType::Ptr &declaredType);

        /**
         * This function is called at runtime when the external variable by name @p name needs
         * to be evaluated. It is not defined how many times this function will be called. It
         * depends on aspects such as how the query was optimized.
         *
         * @param name the name of the variable. Quaranteed to never be @c null.
         * @param context the DynamicContext.
         * @returns the value of the variable. Remember that this value must match the
         * sequence type returned from announceExternalVariable() for the same name.
         */
        virtual Item::Iterator::Ptr evaluateSequence(const QXmlName name,
                                                     const QExplicitlySharedDataPointer<DynamicContext> &context);

        virtual Item evaluateSingleton(const QXmlName name,
                                            const QExplicitlySharedDataPointer<DynamicContext> &context);
        virtual bool evaluateEBV(const QXmlName name,
                                 const QExplicitlySharedDataPointer<DynamicContext> &context);
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
