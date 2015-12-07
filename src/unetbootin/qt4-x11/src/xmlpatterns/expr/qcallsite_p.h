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

#ifndef Patternist_CallSite_H
#define Patternist_CallSite_H

#include "qunlimitedcontainer_p.h"
#include "qcalltargetdescription_p.h"
#include "qxmlname.h"

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Abstract base-class for Expression instances that are callsites
     * to other components, such as templates or user functions.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_expressions
     * @since 4.5
     */
    class CallSite : public UnlimitedContainer
    {
    public:
        QXmlName name() const;
        bool isRecursive() const;
        void setIsRecursive(const bool value);

        /**
         * Called in the earliest stages of the compilation process. @p sign can
         * be any function signature for a user declared function. If @p sign
         * matches this UserFunctionCallsite, it means the UserFunction represented
         * by @p sign is recursive and that this UserFunctionCallsite should take
         * appropriate measures.
         *
         * @returns @c true if is recursive, otherwise @c false
         */
        virtual bool configureRecursion(const CallTargetDescription::Ptr &sign) = 0;

        /**
         * @short Returns the body of the function/template/component that is
         * being called.
         */
        virtual Expression::Ptr body() const = 0;

        virtual CallTargetDescription::Ptr callTargetDescription() const = 0;

    protected:
        CallSite(const QXmlName &name = QXmlName());

    private:
        Q_DISABLE_COPY(CallSite)
        bool            m_isRecursive;
        const QXmlName  m_name;
    };
}

QT_END_NAMESPACE
QT_END_HEADER

#endif
