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

#ifndef Patternist_TemplateMode_H
#define Patternist_TemplateMode_H

#include <QtCore/QSharedData>
#include <QXmlName>

#include "qtemplatepattern_p.h"

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Houses the data specific to the templates for a certain mode.
     *
     * @see Template
     * @see TemplatePattern
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_expressions
     * @since 4.5
     */
    class Q_AUTOTEST_EXPORT TemplateMode : public QSharedData
    {
    public:
        typedef QExplicitlySharedDataPointer<TemplateMode> Ptr;

        inline TemplateMode(const QXmlName &modeName) : m_modeName(modeName)
        {
        }

        TemplatePattern::Vector templatePatterns;

        /**
         * Adds the templates in @p mode to this TemplateMode.
         *
         * The existing name remains.
         */
        inline void addMode(const TemplateMode::Ptr &mode);

        inline const QXmlName &name() const;

        /**
         * Orders its templates by priority such that the first lookup always
         * returns the template with highest priority, and removes templates
         * shadowed by import precedence.
         */
        void finalize();

    private:
        const QXmlName m_modeName;
        Q_DISABLE_COPY(TemplateMode)

        /**
         * Operator for qSort().
         */
        static inline bool lessThanByPriority(const TemplatePattern::Ptr &t1,
                                              const TemplatePattern::Ptr &t2);
    };

    const QXmlName &TemplateMode::name() const
    {
        return m_modeName;
    }

    void TemplateMode::addMode(const TemplateMode::Ptr &mode)
    {
        templatePatterns += mode->templatePatterns;
    }

}

QT_END_NAMESPACE

QT_END_HEADER

#endif
