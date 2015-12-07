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

#include <QtCore/QFile>
#include <QtCore/QTextCodec>
#include <QtCore/QTimer>
#include <QtCore/QXmlStreamReader>

#include <QtNetwork/QNetworkRequest>

#include "qacceltreebuilder_p.h"
#include "qatomicstring_p.h"
#include "qautoptr_p.h"
#include "qcommonsequencetypes_p.h"

#include "qacceltreeresourceloader_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

static inline uint qHash(const QUrl &uri)
{
        return qHash(uri.toString());
}

AccelTreeResourceLoader::AccelTreeResourceLoader(const NamePool::Ptr &np,
                                                 const NetworkAccessDelegator::Ptr &manager) : m_namePool(np)
                                                                                             , m_networkAccessDelegator(manager)
{
    Q_ASSERT(m_namePool);
    Q_ASSERT(m_networkAccessDelegator);
}

bool AccelTreeResourceLoader::retrieveDocument(const QUrl &uri,
                                               const ReportContext::Ptr &context)
{
    Q_ASSERT(uri.isValid());
    AccelTreeBuilder<true> builder(uri, uri, m_namePool, context.data());

    const AutoPtr<QNetworkReply> reply(load(uri, m_networkAccessDelegator, context));

    if(!reply)
        return false;

    bool success = false;
    success = streamToReceiver(reply.data(), &builder, m_namePool, context, uri);

    m_loadedDocuments.insert(uri, builder.builtDocument());
    return success;
}

QNetworkReply *AccelTreeResourceLoader::load(const QUrl &uri,
                                             const NetworkAccessDelegator::Ptr &networkDelegator,
                                             const ReportContext::Ptr &context)
{
    return load(uri,
                networkDelegator->managerFor(uri),
                context);
}

QNetworkReply *AccelTreeResourceLoader::load(const QUrl &uri,
                                             QNetworkAccessManager *const networkManager,
                                             const ReportContext::Ptr &context)
{
    Q_ASSERT(networkManager);
    Q_ASSERT(uri.isValid());

    NetworkLoop networkLoop;

    QNetworkRequest request(uri);
    QNetworkReply *const reply = networkManager->get(request);
    networkLoop.connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(error(QNetworkReply::NetworkError)));
    networkLoop.connect(reply, SIGNAL(finished()), SLOT(finished()));

    if(networkLoop.exec())
    {
        const QString errorMessage(escape(reply->errorString()));

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
                    context->error(escape(reader.errorString()), ReportContext::FODC0002, QSourceLocation(uri, reader.lineNumber(), reader.columnNumber()));

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

static inline uint qHash(const QPair<QUrl, QString> &desc)
{
    /* Probably a lousy hash. */
    return qHash(desc.first) + qHash(desc.second);
}

bool AccelTreeResourceLoader::retrieveUnparsedText(const QUrl &uri,
                                                   const QString &encoding,
                                                   const ReportContext::Ptr &context,
                                                   const SourceLocationReflection *const where)
{
    const AutoPtr<QNetworkReply> reply(load(uri, m_networkAccessDelegator, context));

    if(!reply)
        return false;

    const QTextCodec * codec;
    if(encoding.isEmpty())
    {
        /* XSL Transformations (XSLT) Version 2.0 16.2 Reading Text Files:
         *
         * "if the media type of the resource is text/xml or application/xml
         * (see [RFC2376]), or if it matches the conventions text/\*+xml or
         * application/\*+xml (see [RFC3023] and/or its successors), then the
         * encoding is recognized as specified in [XML 1.0]"
         */
        codec = QTextCodec::codecForMib(106);
    }
    else
    {
        codec = QTextCodec::codecForName(encoding.toLatin1());
        if(codec && context)
        {
            context->error(QtXmlPatterns::tr("%1 is an unsupported encoding.").arg(formatURI(encoding)),
                           ReportContext::XTDE1190,
                           where);
        }
        else
            return false;
    }

    QTextCodec::ConverterState converterState;
    const QByteArray inData(reply->readAll());
    const QString result(codec->toUnicode(inData.constData(), inData.length(), &converterState));

    if(converterState.invalidChars)
    {
        if(context)
        {
            context->error(QtXmlPatterns::tr("%1 contains octets which are disallowed in "
                                             "the requested encoding %2.").arg(formatURI(uri),
                                                                               formatURI(encoding)),
                           ReportContext::XTDE1190,
                           where);
        }
        else
            return false;
    }

    const int len = result.length();
    /* This code is a candidate for threading. Divide and conqueror. */
    for(int i = 0; i < len; ++i)
    {
        if(!QXmlUtils::isChar(result.at(i)))
        {
            if(context)
            {
                context->error(QtXmlPatterns::tr("The codepoint %1, occurring in %2 using encoding %3, "
                                                 "is an invalid XML character.").arg(formatData(result.at(i)),
                                                                                     formatURI(uri),
                                                                                     formatURI(encoding)),
                               ReportContext::XTDE1190,
                               where);
            }
            else
                return false;
        }
    }

    m_unparsedTexts.insert(qMakePair(uri, encoding), result);
    return true;
}

bool AccelTreeResourceLoader::isUnparsedTextAvailable(const QUrl &uri,
                                                      const QString &encoding)
{
    return retrieveUnparsedText(uri, encoding, ReportContext::Ptr(), 0);
}

Item AccelTreeResourceLoader::openUnparsedText(const QUrl &uri,
                                               const QString &encoding,
                                               const ReportContext::Ptr &context,
                                               const SourceLocationReflection *const where)
{
    const QString &text = m_unparsedTexts.value(qMakePair(uri, encoding));

    if(text.isNull())
    {
        if(retrieveUnparsedText(uri, encoding, context, where))
            return openUnparsedText(uri, encoding, context, where);
        else
            return Item();
    }
    else
        return AtomicString::fromValue(text);
}

QSet<QUrl> AccelTreeResourceLoader::deviceURIs() const
{
     QHash<QUrl, AccelTree::Ptr>::const_iterator it(m_loadedDocuments.constBegin());
     const QHash<QUrl, AccelTree::Ptr>::const_iterator end(m_loadedDocuments.constEnd());
     QSet<QUrl> retval;

     while (it != end)
     {
         if(it.key().toString().startsWith(QLatin1String("tag:trolltech.com,2007:QtXmlPatterns:QIODeviceVariable:")))
             retval.insert(it.key());

         ++it;
     }

     return retval;
}

void AccelTreeResourceLoader::clear(const QUrl &uri)
{
    m_loadedDocuments.remove(uri);
}

QT_END_NAMESPACE

