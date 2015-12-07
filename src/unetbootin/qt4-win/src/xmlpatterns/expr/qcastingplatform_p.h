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

#ifndef Patternist_CastingPlatform_H
#define Patternist_CastingPlatform_H

#include "qatomiccaster_p.h"
#include "qqnamevalue_p.h"
#include "qatomicstring_p.h"
#include "qvalidationerror_p.h"
#include "qatomiccasterlocator_p.h"
#include "qatomictype_p.h"
#include "qbuiltintypes_p.h"
#include "qcommonsequencetypes_p.h"
#include "qschematypefactory_p.h"
#include "qpatternistlocale_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Provides casting functionality for classes, such as CastAs or NumberFN, which
     * needs to perform casting.
     *
     * Classes which need to perform casting can simply from this class and gain
     * access to casting functinality wrapped in a convenient way. At the center of this
     * class is the cast() function, which is used at runtime to perform the actual cast.
     *
     * The actual circumstances where casting is used, such as in the 'castable as'
     * expression or the <tt>fn:number()</tt> function, often have other things to handle as well,
     * error handling and cardinality checks for example. This class handles only casting
     * and leaves the other case-specific details to the sub-class such that this class only
     * do one thing well.
     *
     * This template class takes two parameters:
     * - TSubClass This should be the class inheriting from CastingPlatform.
     * - issueError if true, errors are issued via ReportContext, otherwise
     *   ValidationError instances are returned appropriately.
     *
     * The class inheriting CastingPlatform must implement the following function:
     * @code
     * ItemType::Ptr targetType() const
     * @endcode
     *
     * that returns the type that should be cast to. The type must be an AtomicType.
     * Typically, it is appropriate to declare this function @c inline.
     *
     * A sub-class calls prepareCasting() at compile time(such that CastingPlatform can attempt
     * to lookup the proper AtomicCaster) and then it simply uses the cast() function at runtime. The
     * function targetType() must be implemented such that CastingPlatform knows
     * what type it shall cast to.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_expressions
     */
    template<typename TSubClass, const bool issueError>
    class CastingPlatform
    {
    protected:
        /**
         * Default constructor. Does nothing. It is implemented in order make template
         * instantiation easier.
         */
        inline CastingPlatform()
        {
        }

        /**
         * Attempts to cast @p sourceValue to targetType(), and returns
         * the created value. Remember that prepareCasting() should have been
         * called at compile time, otherwise this function will be slow.
         *
         * Error reporting is done in two ways. If a cast fails because
         * of an error in lexical representation a ValidationError is returned.
         * If the cause of failure is that the casting combination is invalid(such as
         * when attempting to cast @c xs:date to @c xs:integer), a ValidationError
         * is returned if @c false was passed in the template instantiation,
         * an error is issued via @p context.
         *
         * @param sourceValue the value to cast. Must be non @c null.
         * @param context the usual DynamicContext, used for error reporting.
         * @returns the new value which was the result of the cast. If the
         * cast failed, an ValidationError is returned.
         */
        Item cast(const Item &sourceValue,
                  const DynamicContext::Ptr &context) const;

        /**
         * This function should be called at compiled time, it attempts to determine
         * what AtomicCaster that should be used when casting from @p sourceType to
         * targetType(). If that is not possible, because the @p sourceType is
         * @c xs:anyAtomicType for instance, the AtomicCaster lookup will done at
         * runtime on a case-per-case basis.
         *
         * @returns @c true if the requested casting combination is valid or might be valid.
         * If it is guranteed to be invalid, @c false is returned.
         */
        bool prepareCasting(const StaticContext::Ptr &context,
                            const ItemType::Ptr &sourceType);

        /**
         * Checks that the targetType() is a valid target type for <tt>castable as</tt>
         * and <tt>cast as</tt>. For example, that it is not abstract. If the type is
         * invalid, an error is raised via the @p context. Note that it is assumed the type
         * is atomic.
         */
        void checkTargetType(const StaticContext::Ptr &context) const;

    private:
        inline Item castWithCaster(const Item &sourceValue,
                                   const AtomicCaster::Ptr &caster,
                                   const DynamicContext::Ptr &context) const;

        /**
         * Locates the caster for casting values of type @p sourceType to targetType(), if
         * possible.
         *
         * @p castImpossible is not initialized. Initialize it to @c false.
         */
        AtomicCaster::Ptr locateCaster(const ItemType::Ptr &sourceType,
                                       const ReportContext::Ptr &context,
                                       bool &castImpossible) const;

        inline ItemType::Ptr targetType() const
        {
            Q_ASSERT(static_cast<const TSubClass *>(this)->targetType());
            return static_cast<const TSubClass *>(this)->targetType();
        }

        void issueCastError(const Item &validationError,
                            const Item &sourceValue,
                            const DynamicContext::Ptr &context) const;

        Q_DISABLE_COPY(CastingPlatform)
        AtomicCaster::Ptr m_caster;
    };

#include "qcastingplatform.cpp"

}

QT_END_NAMESPACE

QT_END_HEADER

#endif
