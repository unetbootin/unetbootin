/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
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

#include <QVariant>
#include <QStringList>

#include "qanyuri_p.h"
#include "qatomicstring_p.h"
#include "qbuiltintypes_p.h"
#include "qcommonsequencetypes_p.h"
#include "qgenericsequencetype_p.h"
#include "qinteger_p.h"
#include "qitem_p.h"
#include "qsequencetype_p.h"
#include "qvariableloader_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

namespace QPatternist
{

    class VariantListIterator : public ListIteratorPlatform<QVariant, Item, VariantListIterator>
    {
    public:
        inline VariantListIterator(const QVariantList &list) : ListIteratorPlatform<QVariant, Item, VariantListIterator>(list)
        {
        }

    private:
        friend class ListIteratorPlatform<QVariant, Item, VariantListIterator>;

        inline Item inputToOutputItem(const QVariant &inputType) const
        {
            return AtomicValue::toXDM(inputType);
        }
    };

    class StringListIterator : public ListIteratorPlatform<QString, Item, StringListIterator>
    {
    public:
        inline StringListIterator(const QStringList &list) : ListIteratorPlatform<QString, Item, StringListIterator>(list)
        {
        }

    private:
        friend class ListIteratorPlatform<QString, Item, StringListIterator>;

        static inline Item inputToOutputItem(const QString &inputType)
        {
            return AtomicString::fromValue(inputType);
        }
    };
}

SequenceType::Ptr VariableLoader::announceExternalVariable(const QXmlName name,
                                                           const SequenceType::Ptr &declaredType)
{
    Q_UNUSED(declaredType);
    const QXmlItem &item = m_bindingHash.value(name);

    if(item.isNull())
    {
        if(m_deviceVariables.contains(name))
            return CommonSequenceTypes::ExactlyOneAnyURI;
        else
            return SequenceType::Ptr();
    }
    else
        return makeGenericSequenceType(QPatternist::AtomicValue::qtToXDMType(item), QPatternist::Cardinality::exactlyOne());
}

Item::Iterator::Ptr VariableLoader::evaluateSequence(const QXmlName name,
                                                     const DynamicContext::Ptr &)
{
    const QXmlItem &item = m_bindingHash.value(name);
    /* Item can be null here, since it's maybe a variable that we have in m_deviceVariables. */
    const QVariant v(item.toAtomicValue());

    switch(v.type())
    {
        case QVariant::StringList:
            return Item::Iterator::Ptr(new StringListIterator(v.toStringList()));
        case QVariant::List:
            return Item::Iterator::Ptr(new VariantListIterator(v.toList()));
        default:
            return makeSingletonIterator(itemForName(name));
    }
}

QPatternist::Item VariableLoader::itemForName(const QXmlName &name) const
{
    const QXmlItem &item = m_bindingHash.value(name);

    if(item.isNode())
        return Item::fromPublic(item);
    else
    {
        const QVariant atomicValue(item.toAtomicValue());
        /* If the atomicValue is null it means it doesn't exist in m_bindingHash, and therefore it must
         * be a QIODevice, since Patternist guarantees to only ask for variables that announceExternalVariable()
         * has accepted. */
        if(atomicValue.isNull())
            return Item(AnyURI::fromValue(QLatin1String("tag:trolltech.com,2007:QtXmlPatterns:QIODeviceVariable:") + m_namePool->stringForLocalName(name.localName())));
        else
            return AtomicValue::toXDM(atomicValue);
    }
}

Item VariableLoader::evaluateSingleton(const QXmlName name,
                                       const DynamicContext::Ptr &)
{
    return itemForName(name);
}

QT_END_NAMESPACE

