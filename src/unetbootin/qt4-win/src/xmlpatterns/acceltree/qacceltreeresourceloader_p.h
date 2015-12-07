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

#ifndef Patternist_AccelTreeResourceLoader_H
#define Patternist_AccelTreeResourceLoader_H

#include <QtCore/QHash>
#include <QtCore/QEventLoop>
#include <QtNetwork/QNetworkReply>

#include "qacceltree_p.h"
#include "qnamepool_p.h"
#include "qreportcontext_p.h"
#include "qresourceloader_p.h"
#include "qabstractxmlreceiver.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QIODevice;

namespace QPatternist
{
    class NetworkLoop : public QEventLoop
    {
        Q_OBJECT
    public:
        NetworkLoop() : m_hasReceivedError(false)
        {
        }

    public Q_SLOTS:
        void error(QNetworkReply::NetworkError code)
        {
            Q_UNUSED(code);
            m_hasReceivedError = true;
            exit(1);
        }

        void finished()
        {
            if(m_hasReceivedError)
                exit(1);
            else
                exit(0);
        }
    private:
        bool m_hasReceivedError;
    };

    /**
     * @short Handles requests for documents, and instantiates
     * them as AccelTree instances.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class Q_AUTOTEST_EXPORT AccelTreeResourceLoader : public ResourceLoader
    {
    public:
        /**
         * AccelTreeResourceLoader does not own @p networkManager.
         */
        AccelTreeResourceLoader(const NamePool::Ptr &np,
                                QNetworkAccessManager *const networkManager,
                                const ReportContext::Ptr &context);

        virtual Item openDocument(const QUrl &uri,
                                  const ReportContext::Ptr &context);
        virtual SequenceType::Ptr announceDocument(const QUrl &uri, const Usage usageHint);
        virtual bool isDocumentAvailable(const QUrl &uri);

        /**
         * Helper function that basically do QNetworkAccessManager::get(), but
         * does it blocked.
         *
         * The return QNetworkReply has emitted QNetworkReply::finished().
         *
         * The caller owns the return QIODevice instance.
         *
         * @p context may be @c null or valid.
         *
         * Calls ReportContext::error() on failure.
         */
        static QNetworkReply *load(const QUrl &uri,
                                   QNetworkAccessManager *const networkManager,
                                   const ReportContext::Ptr &context);

    private:
        static bool streamToReceiver(QIODevice *const dev,
                                     QAbstractXmlReceiver *const receiver,
                                     const NamePool::Ptr &np,
                                     const ReportContext::Ptr &context,
                                     const QUrl &uri);
        bool retrieveDocument(const QUrl &uri,
                              const ReportContext::Ptr &context);

        QHash<QUrl, AccelTree::Ptr>     m_loadedDocuments;
        const NamePool::Ptr             m_namePool;
        QNetworkAccessManager *const    m_networkAccessManager;
        const ReportContext::Ptr        m_context;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
