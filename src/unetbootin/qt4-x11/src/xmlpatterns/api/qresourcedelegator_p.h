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

#ifndef QPatternist_ResourceDelegator_p_H
#define QPatternist_ResourceDelegator_p_H

#include <QSet>
#include <QUrl>

#include "qdeviceresourceloader_p.h"

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Delegates to another ResourceLoader, but in case a URI is in an
     * exception list, it delegates to a different loader.
     *
     * This is used for handling device variables, since when a device variable
     * is rebound, a resource loader needs to carry that binding, while the
     * resource loader for the other query remains as is.
     *
     * @since 4.5
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class ResourceDelegator : public DeviceResourceLoader
    {
    public:
        ResourceDelegator(const QSet<QUrl> &needsOverride,
                          const ResourceLoader::Ptr &parentLoader,
                          const ResourceLoader::Ptr &forDeviceLoader) : m_needsOverride(needsOverride)
                                                                      , m_parentLoader(parentLoader)
                                                                      , m_forDeviceLoader(forDeviceLoader)

        {
            Q_ASSERT(m_parentLoader);
        }

        virtual bool isUnparsedTextAvailable(const QUrl &uri,
                                             const QString &encoding);
        virtual ItemType::Ptr announceUnparsedText(const QUrl &uri);
        virtual Item openUnparsedText(const QUrl &uri,
                                      const QString &encoding,
                                      const ReportContext::Ptr &context,
                                      const SourceLocationReflection *const where);
        virtual Item openDocument(const QUrl &uri,
                                  const ReportContext::Ptr &context);
        virtual SequenceType::Ptr announceDocument(const QUrl &uri, const Usage usageHint);
        virtual bool isDocumentAvailable(const QUrl &uri);
        virtual Item::Iterator::Ptr openCollection(const QUrl &uri);
        virtual SequenceType::Ptr announceCollection(const QUrl &uri);

        /**
         * Returns the union of the deviceURIs() that ResourceDelegator's two
         * resource loaders has.
         */
        virtual QSet<QUrl> deviceURIs() const;

    private:
        const QSet<QUrl> m_needsOverride;
        const ResourceLoader::Ptr m_parentLoader;
        const ResourceDelegator::Ptr m_forDeviceLoader;
    };
}

QT_END_NAMESPACE
QT_END_HEADER

#endif
