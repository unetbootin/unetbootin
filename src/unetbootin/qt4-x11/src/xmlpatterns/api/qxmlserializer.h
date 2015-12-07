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

#ifndef QXMLSERIALIZER_H
#define QXMLSERIALIZER_H

#include <QtXmlPatterns/QAbstractXmlReceiver>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(XmlPatterns)

class QIODevice;
class QTextCodec;
class QXmlQuery;
class QXmlSerializerPrivate;

class Q_XMLPATTERNS_EXPORT QXmlSerializer : public QAbstractXmlReceiver
{
public:
    QXmlSerializer(const QXmlQuery &query,
                   QIODevice *outputDevice);

    virtual void namespaceBinding(const QXmlName &nb);

    virtual void characters(const QStringRef &value);
    virtual void comment(const QString &value);

    virtual void startElement(const QXmlName &name);

    virtual void endElement();

    virtual void attribute(const QXmlName &name,
                           const QStringRef &value);

    virtual void processingInstruction(const QXmlName &name,
                                       const QString &value);

    virtual void atomicValue(const QVariant &value);

    virtual void startDocument();
    virtual void endDocument();
    virtual void startOfSequence();
    virtual void endOfSequence();

    QIODevice *outputDevice() const;

    void setCodec(const QTextCodec *codec);
    const QTextCodec *codec() const;

    /* The members below are internal, not part of the public API, and
     * unsupported. Using them leads to undefined behavior. */
    virtual void item(const QPatternist::Item &item);
protected:
    QXmlSerializer(QAbstractXmlReceiverPrivate *d);

private:
    inline bool isBindingInScope(const QXmlName nb) const;

    /**
     * Where in the document the QXmlSerializer is currently working.
     */
    enum State
    {
        /**
         * Before the document element. This is the XML prolog where the
         * XML declaration, and possibly comments and processing
         * instructions are found.
         */
        BeforeDocumentElement,

        /**
         * This is inside the document element, at any level.
         */
        InsideDocumentElement
    };

    /**
     * If the current state is neither BeforeDocumentElement or
     * AfterDocumentElement.
     */
    inline bool atDocumentRoot() const;

    /**
     * Closes any open element start tag. Must be called before outputting
     * any element content.
     */
    inline void startContent();

    /**
     * Escapes content intended as text nodes for elements.
     */
    void writeEscaped(const QString &toEscape);

    /**
     * Identical to writeEscaped(), but also escapes quotes.
     */
    inline void writeEscapedAttribute(const QString &toEscape);

    /**
     * Writes out @p name.
     */
    inline void write(const QXmlName &name);

    inline void write(const char *const chars);
    /**
     * Encodes and writes out @p content.
     */
    inline void write(const QString &content);

    Q_DECLARE_PRIVATE(QXmlSerializer)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
