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

#ifndef Patternist_OutputValidator_H
#define Patternist_OutputValidator_H

#include <QSet>

#include "qdynamiccontext_p.h"
#include "qabstractxmlreceiver.h"
#include "qsourcelocationreflection_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Receives QAbstractXmlReceiver events and validates that they are correct,
     * before sending them on to a second QAbstractXmlReceiver.
     *
     * Currently, this is only checking that attributes appear before other
     * nodes.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     * @todo Escape data
     */
    class OutputValidator : public QAbstractXmlReceiver
                          , public DelegatingSourceLocationReflection
    {
    public:
        OutputValidator(QAbstractXmlReceiver *const receiver,
                        const DynamicContext::Ptr &context,
                        const SourceLocationReflection *const r,
                        const bool isXSLT);

        virtual void namespaceBinding(const QXmlName &nb);

        virtual void characters(const QStringRef &value);
        virtual void comment(const QString &value);

        virtual void startElement(const QXmlName &name);

        virtual void endElement();

        virtual void attribute(const QXmlName &name,
                               const QStringRef &value);

        virtual void processingInstruction(const QXmlName &name,
                                           const QString &value);

        virtual void item(const Item &item);

        virtual void startDocument();
        virtual void endDocument();
        virtual void atomicValue(const QVariant &value);
        virtual void endOfSequence();
        virtual void startOfSequence();

    private:
        bool                        m_hasReceivedChildren;
        QAbstractXmlReceiver *const m_receiver;
        const DynamicContext::Ptr   m_context;

        /**
         * Keeps the current received attributes, in order to check uniqueness.
         */
        QSet<QXmlName>              m_attributes;
        const bool                  m_isXSLT;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
