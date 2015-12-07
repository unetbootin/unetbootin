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

/**
 * @file
 * @short This file is included by qcastingplatform_p.h.
 * If you need includes in this file, put them in CasttingPlatform.h, outside of the namespace.
 */

template <typename TSubClass, const bool issueError>
Item CastingPlatform<TSubClass, issueError>::castWithCaster(const Item &sourceValue,
                                                            const AtomicCaster::Ptr &caster,
                                                            const ReportContext::Ptr &context) const
{
    Q_ASSERT(sourceValue);
    Q_ASSERT(caster);
    Q_ASSERT(context);

    const Item retval(caster->castFrom(sourceValue, context));

    if(issueError)
    {
        if(retval.template as<AtomicValue>()->hasError())
        {
            issueCastError(retval, sourceValue, context);
            return Item();
        }
        else
            return retval;
    }
    else
        return retval;
}

template <typename TSubClass, const bool issueError>
Item CastingPlatform<TSubClass, issueError>::cast(const Item &sourceValue,
                                                  const ReportContext::Ptr &context) const
{
    Q_ASSERT(sourceValue);
    Q_ASSERT(context);
    Q_ASSERT(targetType());

    if(m_caster)
        return castWithCaster(sourceValue, m_caster, context);
    else
    {
        bool castImpossible = false;
        const AtomicCaster::Ptr caster(locateCaster(sourceValue.type(), context, castImpossible));

        if(!issueError && castImpossible)
        {
            /* If we're supposed to issue an error(issueError) then this
             * line will never be reached, because locateCaster() will in
             * that case throw. */
            return ValidationError::createError();
        }
        else
            return castWithCaster(sourceValue, caster, context);
    }
}

template <typename TSubClass, const bool issueError>
bool CastingPlatform<TSubClass, issueError>::prepareCasting(const ReportContext::Ptr &context,
                                                            const ItemType::Ptr &sourceType)
{
    Q_ASSERT(sourceType);
    Q_ASSERT(context);

    if(*sourceType == *BuiltinTypes::xsAnyAtomicType ||
       *sourceType == *BuiltinTypes::item ||
       *sourceType == *CommonSequenceTypes::Empty ||
       *sourceType == *BuiltinTypes::numeric)
        return true; /* The type could not be narrowed better than xs:anyAtomicType
                        or numeric at compile time. We'll do lookup at runtime instead. */

    bool castImpossible = false;
    m_caster = locateCaster(sourceType, context, castImpossible);

    return !castImpossible;
}

template <typename TSubClass, const bool issueError>
AtomicCaster::Ptr CastingPlatform<TSubClass, issueError>::locateCaster(const ItemType::Ptr &sourceType,
                                                                       const ReportContext::Ptr &context,
                                                                       bool &castImpossible) const
{
    Q_ASSERT(sourceType);
    Q_ASSERT(targetType());

    const AtomicCasterLocator::Ptr locator(static_cast<AtomicType *>(
            targetType().data())->casterLocator());
    if(!locator)
    {
        if(issueError)
        {
            context->error(QtXmlPatterns::tr("No casting is possible with %1 as the target type.")
                                        .arg(formatType(context->namePool(), targetType())),
                                       ReportContext::XPTY0004, static_cast<const TSubClass *>(this));
        }
        else
            castImpossible = true;

        return AtomicCaster::Ptr();
    }

    const AtomicCaster::Ptr caster(static_cast<const AtomicType *>(sourceType.data())->accept(locator, static_cast<const TSubClass *>(this)));
    if(!caster)
    {
        if(issueError)
        {
            context->error(QtXmlPatterns::tr("It is not possible to cast from %1 to %2.")
                                            .arg(formatType(context->namePool(), sourceType))
                                            .arg(formatType(context->namePool(), targetType())),
                                       ReportContext::XPTY0004, static_cast<const TSubClass *>(this));
        }
        else
            castImpossible = true;

        return AtomicCaster::Ptr();
    }

    return caster;
}

template <typename TSubClass, const bool issueError>
void CastingPlatform<TSubClass, issueError>::checkTargetType(const ReportContext::Ptr &context) const
{
    Q_ASSERT(context);

    const ItemType::Ptr tType(targetType());
    Q_ASSERT(tType);
    Q_ASSERT(tType->isAtomicType());
    const AtomicType::Ptr asAtomic(tType);

    /* This catches casting to xs:NOTATION and xs:anyAtomicType. */
    if(asAtomic->isAbstract())
    {
        context->error(QtXmlPatterns::tr("Casting to %1 is not possible because it "
                                     "is an abstract type, and can therefore never be instantiated.")
                                .arg(formatType(context->namePool(), tType)),
                          ReportContext::XPST0080,
                          static_cast<const TSubClass*>(this));
    }
}

template <typename TSubClass, const bool issueError>
void CastingPlatform<TSubClass, issueError>::issueCastError(const Item &validationError,
                                                            const Item &sourceValue,
                                                            const ReportContext::Ptr &context) const
{
    Q_ASSERT(validationError);
    Q_ASSERT(context);
    Q_ASSERT(validationError.isAtomicValue());
    Q_ASSERT(validationError.template as<AtomicValue>()->hasError());

    const ValidationError::Ptr err(validationError.template as<ValidationError>());
    QString msg(err->message());

    if(msg.isNull())
    {
        msg = QtXmlPatterns::tr("It's not possible to cast the value %1 of type %2 to %3")
                 .arg(formatData(sourceValue.stringValue()))
                 .arg(formatType(context->namePool(), sourceValue.type()))
                 .arg(formatType(context->namePool(), targetType()));
    }
    else
    {
        Q_ASSERT(!msg.isEmpty());
        msg = QtXmlPatterns::tr("Failure when casting from %1 to %2: %3")
                 .arg(formatType(context->namePool(), sourceValue.type()))
                 .arg(formatType(context->namePool(), targetType()))
                 .arg(msg);
    }

    /* If m_errorCode is FORG0001, we assume our sub-classer doesn't have a
     * special wish about error code, so then we use the error object's code.
     */
    context->error(msg, m_errorCode == ReportContext::FORG0001 ? err->errorCode() : m_errorCode,
                   static_cast<const TSubClass*>(this));
}

