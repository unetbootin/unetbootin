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

#ifndef Patternist_DelegatingStaticContext_H
#define Patternist_DelegatingStaticContext_H

#include <QUrl>

#include "qstaticcontext_p.h"
#include "qfunctionfactory_p.h"
#include "qschematypefactory_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Delegates all members to a second instance. Used for
     * sub-classing.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class Q_AUTOTEST_EXPORT DelegatingStaticContext : public StaticContext
    {
    public:
        virtual NamespaceResolver::Ptr namespaceBindings() const;
        virtual void setNamespaceBindings(const NamespaceResolver::Ptr &);

        virtual FunctionFactory::Ptr functionSignatures() const;
        virtual SchemaTypeFactory::Ptr schemaDefinitions() const;
        virtual DynamicContext::Ptr dynamicContext() const;

        virtual QUrl baseURI() const;
        virtual void setBaseURI(const QUrl &uri);

        virtual bool compatModeEnabled() const;
        virtual void setCompatModeEnabled(const bool newVal);

        virtual QUrl defaultCollation() const;

        virtual QAbstractMessageHandler * messageHandler() const;

        virtual void setDefaultCollation(const QUrl &uri);

        virtual BoundarySpacePolicy boundarySpacePolicy() const;
        virtual void setBoundarySpacePolicy(const BoundarySpacePolicy policy);

        virtual ConstructionMode constructionMode() const;
        virtual void setConstructionMode(const ConstructionMode mode);

        virtual OrderingMode orderingMode() const;
        virtual void setOrderingMode(const OrderingMode mode);
        virtual OrderingEmptySequence orderingEmptySequence() const;
        virtual void setOrderingEmptySequence(const OrderingEmptySequence ordering);

        virtual QString defaultFunctionNamespace() const;
        virtual void setDefaultFunctionNamespace(const QString &ns);

        virtual QString defaultElementNamespace() const;
        virtual void setDefaultElementNamespace(const QString &ns);

        virtual InheritMode inheritMode() const;
        virtual void setInheritMode(const InheritMode mode);

        virtual PreserveMode preserveMode() const;
        virtual void setPreserveMode(const PreserveMode mode);

        virtual ItemType::Ptr contextItemType() const;
        virtual ItemType::Ptr currentItemType() const;

        virtual StaticContext::Ptr copy() const;

        virtual ExternalVariableLoader::Ptr externalVariableLoader() const;
        virtual ResourceLoader::Ptr resourceLoader() const;
        virtual NamePool::Ptr namePool() const;
        virtual void addLocation(const SourceLocationReflection *const reflection,
                                 const QSourceLocation &location);
        virtual LocationHash sourceLocations() const;
        virtual QSourceLocation locationFor(const SourceLocationReflection *const reflection) const;
        virtual const QAbstractUriResolver *uriResolver() const;

        virtual VariableSlotID currentRangeSlot() const;
        virtual VariableSlotID allocateRangeSlot();

    protected:
        DelegatingStaticContext(const StaticContext::Ptr &context);

    private:
        const StaticContext::Ptr    m_context;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
