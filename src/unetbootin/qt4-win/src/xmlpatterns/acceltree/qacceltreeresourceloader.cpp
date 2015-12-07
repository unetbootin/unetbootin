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

#include <QtCore/QEventLoop>
#include <QtCore/QFile>
#include <QtCore/QTimer>
#include <QtCore/QXmlStreamReader>

#include <QtNetwork/QNetworkRequest>

#include "qacceltreebuilder_p.h"
#include "qcommonsequencetypes_p.h"

#include "qacceltreeresourceloader_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

static inline uint qHash(const QUrl &uri)
{
        return qHash(uri.toString());
}

AccelTreeResourceLoader::AccelTreeResourceLoader(const NamePool::Ptr &np,
                                                 QNetworkAccessManager *const manager,
                                                 const ReportContext::Ptr &context) : m_namePool(np)
                                                                                    , m_networkAccessManager(manager)
                                                                                    , m_context(context)
{
    Q_ASSERT(m_namePool);
    Q_ASSERT(m_networkAccessManager);
}

bool AccelTreeResourceLoader::retrieveDocument(const QUrl &uri,
                                               const ReportContext::Ptr &context)
{
    Q_ASSERT(uri.isValid());
    AccelTreeBuilder<true> builder(uri, uri, m_namePool, m_context);

    QNetworkReply *const reply = load(uri, m_networkAccessManager, context);

    if(!reply)
        return false;

    bool success = false;
    try
    {
        success = streamToReceiver(reply, &builder, m_namePool, context, uri);
    }
    catch(const QPatternist::Exception)
    {
        delete reply;
        throw; /* Let the exception continue to propagate. */
    }

    delete reply;
    m_loadedDocuments.insert(uri, builder.builtDocument());
    return success;
}

QNetworkReply *AccelTreeResourceLoader::load(const QUrl &uri,
                                             QNetworkAccessManager *const networkManager,
                                             const ReportContext::Ptr &context)
{
    Q_ASSERT(uri.isValid());
    Q_ASSERT(networkManager);

    NetworkLoop networkLoop;

    QNetworkRequest request(uri);
    QNetworkReply *const reply = networkManager->get(request);
    networkLoop.connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(error(QNetworkReply::NetworkError)));
    networkLoop.connect(reply, SIGNAL(finished()), SLOT(finished()));

    if(networkLoop.exec())
    {
        const QString errorMessage(reply->errorString());

        /* Note, we delete reply before we exit this function with error(). */
        delete reply;

        const QSourceLocation location(uri);

        if(context)
            context->error(errorMessage, ReportContext::FODC0002, location);

        return 0;
    }
    else
        return reply;
}

bool AccelTreeResourceLoader::streamToReceiver(QIODevice *const dev,
                                               QAbstractXmlReceiver *const receiver,
                                               const NamePool::Ptr &np,
                                               const ReportContext::Ptr &context,
                                               const QUrl &uri)
{
    Q_ASSERT(dev);
    Q_ASSERT(receiver);
    Q_ASSERT(np);

    QXmlStreamReader reader(dev);

    /* Optimize: change NamePool to take QStringRef such that we don't have to call toString() below. That
     * will save us a gazillion of temporary QStrings. */

    while(!reader.atEnd())
    {
        reader.readNext();

        switch(reader.tokenType())
        {
            case QXmlStreamReader::StartElement:
            {
                /* Send the name. */
                receiver->startElement(np->allocateQName(reader.namespaceUri().toString(), reader.name().toString(),
                                                         reader.prefix().toString()));

                /* Send namespace declarations. */
                const QXmlStreamNamespaceDeclarations &nss = reader.namespaceDeclarations();

                /* The far most common case, is for it to be empty. */
                if(!nss.isEmpty())
                {
                    const int len = nss.size();

                    for(int i = 0; i < len; ++i)
                    {
                        const QXmlStreamNamespaceDeclaration &ns = nss.at(i);
                        receiver->namespaceBinding(np->allocateBinding(ns.prefix().toString(), ns.namespaceUri().toString()));
                    }
                }

                /* Send attributes. */
                const QXmlStreamAttributes &attrs = reader.attributes();
                const int len = attrs.size();

                for(int i = 0; i < len; ++i)
                {
                    const QXmlStreamAttribute &attr = attrs.at(i);

                    receiver->attribute(np->allocateQName(attr.namespaceUri().toString(), attr.name().toString(),
                                                          attr.prefix().toString()),
                                        attr.value());
                }

                continue;
            }
            case QXmlStreamReader::EndElement:
            {
                receiver->endElement();
                continue;
            }
            case QXmlStreamReader::Characters:
            {
                if(reader.isWhitespace())
                    receiver->whitespaceOnly(reader.text());
                else
                    receiver->characters(reader.text());

                continue;
            }
            case QXmlStreamReader::Comment:
            {
                receiver->comment(reader.text().toString());
                continue;
            }
            case QXmlStreamReader::ProcessingInstruction:
            {
                receiver->processingInstruction(np->allocateQName(QString(), reader.processingInstructionTarget().toString()),
                                                reader.processingInstructionData().toString());
                continue;
            }
            case QXmlStreamReader::StartDocument:
            {
                receiver->startDocument();
                continue;
            }
            case QXmlStreamReader::EndDocument:
            {
                receiver->endDocument();
                continue;
            }
            case QXmlStreamReader::EntityReference:
            /* Fallthrough. */
            case QXmlStreamReader::DTD:
            {
                /* We just ignore any DTD and entity references. */
                continue;
            }
            case QXmlStreamReader::Invalid:
            {
                if(context)
                    context->error(reader.errorString(), ReportContext::FODC0002, QSourceLocation(uri, reader.lineNumber(), reader.columnNumber()));

                return false;
            }
            case QXmlStreamReader::NoToken:
            {
                Q_ASSERT_X(false, Q_FUNC_INFO,
                           "This token is never expected to be received.");
                return false;
            }
        }
    }

    return true;
}

Item AccelTreeResourceLoader::openDocument(const QUrl &uri,
                                           const ReportContext::Ptr &context)
{
    const AccelTree::Ptr doc(m_loadedDocuments.value(uri));

    if(doc)
        return doc->root(QXmlNodeModelIndex()); /* Pass in dummy object. We know AccelTree doesn't use it. */
    else
    {
        if(retrieveDocument(uri, context))
            return m_loadedDocuments.value(uri)->root(QXmlNodeModelIndex()); /* Pass in dummy object. We know AccelTree doesn't use it. */
        else
            return Item();
    }
}

SequenceType::Ptr AccelTreeResourceLoader::announceDocument(const QUrl &uri, const Usage)
{
    // TODO deal with the usage thingy
    Q_ASSERT(uri.isValid());
    Q_ASSERT(!uri.isRelative());
    Q_UNUSED(uri); /* Needed when compiling in release mode. */

    return CommonSequenceTypes::ZeroOrOneDocumentNode;
}

bool AccelTreeResourceLoader::isDocumentAvailable(const QUrl &uri)
{
    return retrieveDocument(uri, ReportContext::Ptr());
}

QT_END_NAMESPACE

