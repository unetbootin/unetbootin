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

#ifndef Patternist_BuiltinNodeType_H
#define Patternist_BuiltinNodeType_H

#include "qitem_p.h"
#include "qanynodetype_p.h"
#include "qbuiltintypes_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{

    /**
     * @short Instances of this class represents types that are sub-classes
     * of <tt>node()</tt>, such as <tt>processing-instruction()</tt>.
     *
     * @ingroup Patternist_types
     * @author Frans Englich <fenglich@trolltech.com>
     */
    template <const QXmlNodeModelIndex::NodeKind kind>
    class BuiltinNodeType : public AnyNodeType
    {
    public:
        virtual bool xdtTypeMatches(const ItemType::Ptr &other) const;
        virtual bool itemMatches(const Item &item) const;

        /**
         * @returns for example "text()", depending on what the constructor was passed
         */
        virtual QString displayName(const NamePool::Ptr &np) const;

        virtual ItemType::Ptr xdtSuperType() const;
        virtual ItemType::Ptr atomizedType() const;

        QXmlNodeModelIndex::NodeKind nodeKind() const;

        PatternPriority patternPriority() const;

    protected:
        friend class BuiltinTypes;

        /**
         * This constructor does nothing, but exists in order to make it impossible to
         * instantiate this class from anywhere but from BuiltinTypes.
         */
        BuiltinNodeType();
    };

#include "qbuiltinnodetype.cpp"

}

QT_END_NAMESPACE

QT_END_HEADER

#endif
